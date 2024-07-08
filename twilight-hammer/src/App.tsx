import reactLogo from "./assets/react.svg";
import viteLogo from "./assets/vite.svg";
import cppLogo from "./assets/cpp.svg";
import edgeLogo from "./assets/edge.png";
import "./App.css";
import {
  Button,
  Caption1,
  Input,
  makeStyles,
  Subtitle1,
  Title1,
  tokens,
} from "@fluentui/react-components";
import { call } from "./containers/api";
import { useState } from "react";
import { useTranslation } from "react-i18next";

const useStyles = makeStyles({
  bar: {
    display: "flex",
    alignItems: "center",
    justifyContent: "space-evenly",
  },
  card: {
    display: "flex",
    flexDirection: "column",
    alignItems: "center",
    gap: tokens.spacingVerticalMNudge,
  },
});
function App() {
  const styles = useStyles();
  const { t } = useTranslation();
  const [toSend, SetToSend] = useState<string>("Hello Twilight Hammer!");
  const [received, SetReceived] = useState<string>("");
  return (
    <>
      <div className={styles.bar}>
        <a href="https://vitejs.dev" target="_blank">
          <img src={viteLogo} className="logo" alt="Vite logo" />
        </a>
        <a href="https://react.dev" target="_blank">
          <img src={reactLogo} className="logo react" alt="React logo" />
        </a>
        <a
          href="https://developer.microsoft.com/en-us/microsoft-edge/webview2"
          target="_blank"
        >
          <img src={edgeLogo} className="logo react" alt="WebView2 logo" />
        </a>
        <a
          href="https://learn.microsoft.com/en-us/cpp/windows/overview-of-windows-programming-in-cpp"
          target="_blank"
        >
          <img src={cppLogo} className="logo react" alt="C++ logo" />
        </a>
      </div>
      <Title1>Vite + React + WebView2 + C++</Title1>
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
          {t("send to host")}
        </Button>
        <Subtitle1>
          {t("received")}
          {received}
        </Subtitle1>
      </div>
      <Caption1>{t("learn more")}</Caption1>
    </>
  );
}

export default App;
