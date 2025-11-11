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
  NavDrawer,
  NavDrawerBody,
  NavItem,
  tokens,
} from "@fluentui/react-components";
import {
  bundleIcon,
  Home24Regular,
  Home24Filled,
  Info24Regular,
  Info24Filled,
  Lightbulb24Regular,
  Lightbulb24Filled,
} from "@fluentui/react-icons";
import { call } from "./containers/api";
import { useState } from "react";
import { useTranslation } from "react-i18next";

const useStyles = makeStyles({
  root: {
    display: "flex",
    width: "100vw",
    height: "100vh",
  },
  background: {
    flex: "1",
    display: "flex",
    alignItems: "center",
    justifyContent: "center",
    margin: tokens.spacingVerticalM,
  },
  content: {
    width: "fit-content",
  },
  bar: {
    display: "flex",
    alignItems: "center",
    justifyContent: "space-evenly",
  },
  card: {
    display: "flex",
    flexDirection: "column",
    gap: tokens.spacingVerticalMNudge,
  },
});

const HomeIcon = bundleIcon(Home24Filled, Home24Regular);

const ExampleIcon = bundleIcon(Lightbulb24Filled, Lightbulb24Regular);

const AboutIcon = bundleIcon(Info24Filled, Info24Regular);

const Home = () => {
  const styles = useStyles();
  const { t } = useTranslation();
  return (
    <>
      <div className={styles.bar}>
        <a href="https://vitejs.dev" target="_blank" rel="noreferrer">
          <img src={viteLogo} className="logo" alt="Vite logo" />
        </a>
        <a href="https://react.dev" target="_blank" rel="noreferrer">
          <img src={reactLogo} className="logo react" alt="React logo" />
        </a>
        <a
          href="https://developer.microsoft.com/en-us/microsoft-edge/webview2"
          target="_blank"
          rel="noreferrer"
        >
          <img src={edgeLogo} className="logo react" alt="WebView2 logo" />
        </a>
        <a
          href="https://learn.microsoft.com/en-us/cpp/windows/overview-of-windows-programming-in-cpp"
          target="_blank"
          rel="noreferrer"
        >
          <img src={cppLogo} className="logo react" alt="C++ logo" />
        </a>
      </div>
      <Title1>Vite + React + WebView2 + C++</Title1>
      <br />
      <Caption1>{t("learn more")}</Caption1>
    </>
  );
};

const Example = () => {
  const styles = useStyles();
  const [toSend, SetToSend] = useState<string>("Hello Twilight Hammer!");
  const [received, SetReceived] = useState<string>("");
  const { t } = useTranslation();
  return (
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
  );
};

const About = () => {
  return (
    <>
      <Title1>Twilight Hammer</Title1>
      <br />
      <Caption1>©2024-2025 M1Knight Technology</Caption1>
    </>
  );
};

function App() {
  const styles = useStyles();
  const { t } = useTranslation();
  const [tab, setTab] = useState<string>("home");
  return (
    <div className={styles.root}>
      <NavDrawer
        density="small"
        selectedValue={tab}
        onNavItemSelect={(_, data) => {
          setTab(data.value);
        }}
        open={true}
        type="inline"
      >
        <NavDrawerBody>
          <NavItem icon={<HomeIcon />} value="home">
            {t("nav.home")}
          </NavItem>
          <NavItem icon={<ExampleIcon />} value="example">
            {t("nav.example")}
          </NavItem>
          <NavItem icon={<AboutIcon />} value="about">
            {t("nav.about")}
          </NavItem>
        </NavDrawerBody>
      </NavDrawer>
      <div className={styles.background}>
        <div className={styles.content}>
          {(() => {
            switch (tab) {
              case "home":
                return <Home />;
              case "example":
                return <Example />;
              case "about":
                return <About />;
              default:
                return <></>;
            }
          })()}
        </div>
      </div>
    </div>
  );
}

export default App;
