import { useState } from "react";
import { useTranslation } from "react-i18next";
import {
  Button,
  Input,
  Label,
  makeStyles,
  tokens,
} from "@fluentui/react-components";
import { call } from "../containers/api";

const useStyles = makeStyles({
  card: {
    display: "flex",
    flexDirection: "column",
    gap: tokens.spacingVerticalMNudge,
  },
});

export const SendMessage = () => {
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
        {t("example.send")}
      </Button>
      <Label>{t("example.received")}</Label>
      <Input value={received} disabled />
    </div>
  );
};
