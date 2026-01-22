import React, { useState } from "react";
import { useTranslation } from "react-i18next";
import { Button, Textarea, Text, makeStyles, tokens, mergeClasses } from "@fluentui/react-components";
import { DocumentText24Regular } from "@fluentui/react-icons";
import { call } from "../containers/api";

const useStyles = makeStyles({
  card: {
    display: "flex",
    flexDirection: "column",
    gap: tokens.spacingVerticalMNudge,
    width: "400px",
  },
  preview: {
    width: "100%",
    height: "200px",
    resize: "none",
  },
});

const useMediaObjectStyles = makeStyles({
  main: {
    display: 'flex',
    flexDirection: 'row',
    rowGap: '4px',
    columnGap: '4px',
  },
  text: {
    display: 'flex',
    flexDirection: 'column',
    rowGap: '4px',
    columnGap: '4px',
  },
  emptyMedia: {
    padding: '20px 20px 20px 80px',
  },
  emptyText: {
    width: '100px',
    height: '50px',
  },

  centerMedia: {
    alignItems: 'center',
  },

  verticalMediaObject: {
    display: 'flex',
    flexDirection: 'column',
    rowGap: '4px',
    columnGap: '4px',
    alignItems: 'center',
  },
  centerTextPosition: {
    alignItems: 'center',
  },
  beforeTextPosition: {
    alignItems: 'flex-end',
  },
});

type MediaObjectTypes = {
  media?: React.ReactElement;
  text?: React.ReactElement;
  textPosition?: 'before' | 'below' | 'after';
  textAlignment?: 'start' | 'center';
};

const MediaObject: React.FunctionComponent<MediaObjectTypes> = ({
  media,
  text,
  textPosition = 'after',
  textAlignment = 'start',
}) => {
  const mediaObjectStyles = useMediaObjectStyles();

  const mainClassName = mergeClasses(
    mediaObjectStyles.main,
    textPosition === 'below' && mediaObjectStyles.verticalMediaObject,
    textAlignment === 'center' && mediaObjectStyles.centerMedia,
  );

  const textClassName = mergeClasses(
    mediaObjectStyles.text,
    textPosition === 'below' && mediaObjectStyles.centerTextPosition,
    textPosition === 'before' && mediaObjectStyles.beforeTextPosition,
  );

  return (
    <div className={mainClassName}>
      {(textPosition === 'after' || textPosition === 'below') && media}
      <div className={textClassName}>{text}</div>
      {textPosition === 'before' && media}
    </div>
  );
};

interface IconFileObjectProps {
  filename: string;
  fullPath: string;
}

export const IconFileObject = ({ filename, fullPath }: IconFileObjectProps) => (
  <MediaObject
    media={<DocumentText24Regular />}
    text={
      <>
        <Text size={400} weight="bold">
          {filename}
        </Text>
        <Text size={200}>{fullPath}</Text>
      </>
    }
  />
);

export const OpenFile = () => {
  const styles = useStyles();
  const { t } = useTranslation();
  const [filePath, setFilePath] = useState<string>("");
  const [fileContent, setFileContent] = useState<string>("");

  const handleOpenFile = async () => {
    try {
      // Step 1: Open native file picker
      const paths = await call<{}, string[]>("/file/open", {});
      if (!paths || paths.length === 0) return;

      const path = paths[0]; // Take first file
      setFilePath(path);

      // Step 2: Read file content as text
      const content = await call<{ path: string; mode: "text" }, { mode: string; content: string }>(
        "/file/getContent",
        { path, mode: "text" }
      );

      setFileContent(content.content);
    } catch (err) {
      console.error("Failed to open/read file:", err);
      setFileContent("Error reading file.");
    }
  };

  return (
    <div className={styles.card}>
      <Button onClick={handleOpenFile}>{t("example.open file")}</Button>
      {filePath && (
        <IconFileObject
            filename={filePath.split(/[/\\]/).pop() || filePath}
            fullPath={filePath}
        />
      )}
      <Textarea
        className={styles.preview}
        value={fileContent}
        readOnly
        placeholder={t("example.file preview")}
      />
    </div>
  );
};