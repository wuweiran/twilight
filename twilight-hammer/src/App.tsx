import reactLogo from "./assets/react.svg";
import viteLogo from "/vite.svg";
import "./App.css";
import {
  Button,
  Input,
  makeStyles,
  Subtitle1,
  tokens,
} from "@fluentui/react-components";
import { call } from "./containers/api";
import { useState } from "react";

const useStyles = makeStyles({
  card: {
    display: "flex",
    flexDirection: "column",
    alignItems: "center",
    gap: tokens.spacingVerticalMNudge,
  },
});
function App() {
  const styles = useStyles();
  const [toSend, SetToSend] = useState<string>("Hello Twilight Hammer!");
  const [received, SetReceived] = useState<string>("");
  return (
    <>
      <div>
        <a href="https://vitejs.dev" target="_blank">
          <img src={viteLogo} className="logo" alt="Vite logo" />
        </a>
        <a href="https://react.dev" target="_blank">
          <img src={reactLogo} className="logo react" alt="React logo" />
        </a>
      </div>
      <h1>Vite + React</h1>
      <div className={styles.card}>
        <Input
          value={toSend}
          onChange={(event) => {
            SetToSend(event.target.value);
          }}
        />
        <Button
          onClick={() =>
            call<string, string>("/hello", toSend).then((result) =>
              SetReceived(result)
            )
          }
        >
          Send the above message to host
        </Button>
        <Subtitle1>Received: {received}</Subtitle1>
      </div>
      <p className="read-the-docs">
        Click on the Vite and React logos to learn more
      </p>
    </>
  );
}

export default App;
