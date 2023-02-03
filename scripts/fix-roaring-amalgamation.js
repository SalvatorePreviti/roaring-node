const fs = require("fs");

function fixFile(filePath) {
  const lines = fs.readFileSync(filePath, "utf-8").split("\n");
  for (let i = 0; i < lines.length; i++) {
    const line = lines[i];
    if (line.startsWith("// Created by amalgamation.sh ")) {
      lines[i] = "// Created by amalgamation.sh";
    }
  }
  fs.writeFileSync(filePath, lines.join("\n"));
}

fixFile("src/cpp/CRoaringUnityBuild/roaring.h");
fixFile("src/cpp/CRoaringUnityBuild/roaring.c");
