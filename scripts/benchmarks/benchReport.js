const colors = require("chalk");

const hasIcons = colors.supportsColor.has256;

const icons = {
  success: colors.green(hasIcons ? "✔" : "√"),
  error: hasIcons ? "✖" : "×",
  arrow: hasIcons ? "➔" : "-",
  bullet: hasIcons ? "•" : "*",
};

function printBenchError(suiteName, bench, error) {
  console.log(
    "\n",
    colors.red("-"),
    colors.yellowBright(
      `Suite ${colors.yellow.bold.italic(suiteName)}${
        bench ? `Benchmark ${colors.yellow.bold.italic(bench.name)}` : ""
      } failed`,
    ),
    "-",
    colors.redBright(error),
  );
}

module.exports = {
  printSuiteName(name) {
    console.log(`${colors.cyan(icons.bullet)} ${colors.cyan("suite")} ${colors.cyanBright(name)}`);
  },

  printSuiteReport(report) {
    const cells = { name: 0, ops: 0, rme: 0, runs: 0, diff: 0 };

    for (const bench of report.benchs) {
      cells.name = Math.max(cells.name, bench.name.length);
      cells.ops = Math.max(cells.ops, bench.ops.length);
      cells.rme = Math.max(cells.rme, bench.rme.length);
      cells.runs = Math.max(cells.runs, bench.runs.length);
      cells.diff = Math.max(cells.diff, bench.diff.length);
    }

    for (const detail of report.details) {
      console.log(`  ${colors.gray(detail)}`);
    }

    let errors = 0;
    if (report.error) {
      ++errors;
      printBenchError(report.name, null, report.error);
    }

    for (const bench of report.benchs) {
      if (bench.error) {
        ++errors;
        printBenchError(report.name, bench, bench.error);
      }
    }

    if (errors) {
      console.log();
    }

    for (const bench of report.benchs) {
      const s = [];
      if (bench.aborted || bench.error) {
        const errorKind = colors.red.bold(bench.error ? "error" : "aborted");
        s.push(`  ${colors.red(icons.error)} ${colors.redBright(bench.name.padEnd(cells.name))} ${errorKind}`);
      } else {
        s.push(`  ${colors.green(icons.success)} ${colors.greenBright(bench.name.padEnd(cells.name))}`);
        s.push(` ${colors.cyanBright(bench.ops.padStart(cells.ops))}${colors.cyan(" ops/sec")}`);
        s.push(`±${bench.rme}%`.padStart(cells.rme + 3));
        s.push(`${bench.runs} runs`.padStart(cells.runs + 6));
        if (bench.fastest) {
          s.push(colors.green(colors.italic(" fastest")));
        } else {
          s.push(colors.yellow(bench.diff.padStart(cells.diff + 1)));
        }
      }
      console.log(...s);
    }
    if (report.benchs.length) {
      if (report.fastest) {
        console.log(colors.cyan(`  ${icons.arrow} Fastest is ${colors.greenBright(report.fastest)}\n`));
      } else {
        console.log(colors.gray(`  ${icons.arrow} No winner\n`));
      }
    } else {
      console.log(colors.gray(`  ${icons.arrow} No benchmarks\n`));
    }
  },

  get colorFlags() {
    return colors.supportsColor.hasBasic ? (colors.supportsColor.has256 ? "--color=256" : "--colors") : "";
  },
};
