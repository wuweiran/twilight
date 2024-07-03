type ApiResponse<T> = {
  requestId: string;
  code: number;
  data?: T;
  info?: string;
};

let pendingRequests = new Map<
  string,
  { resolve: (value: ApiResponse<any>) => void; reject: (reason?: any) => void }
>();

function nextRequestId(): string {
  return window.performance.now().toString();
}

// @ts-ignore:next-line
window.chrome.webview.addEventListener("message", (event) => {
  const apiResponse = event.data as ApiResponse<any>;
  const requestId = apiResponse.requestId;
  if (pendingRequests.has(requestId)) {
    pendingRequests.get(requestId)!.resolve(apiResponse);
    pendingRequests.delete(requestId);
  } else {
    console.warn("unexpected message", event.data);
  }
});

export function call<Request, Response>(
  uri: string,
  request: Request
): Promise<Response> {
  return new Promise<ApiResponse<Response>>((resolve, reject) => {
    const requestId = nextRequestId();
    pendingRequests.set(requestId, { resolve, reject });
    // @ts-ignore:next-line
    window.chrome.webview.postMessage({ requestId, uri, data: request });
    setTimeout(() => reject("timeout"), 1000);
  }).then((response) => {
    if (response.code < 0) {
      return Promise.reject(response);
    } else {
      return response.data!;
    }
  });
}
