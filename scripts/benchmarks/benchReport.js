const chalk = require("chalk");

const hasIcons = chalk.supportsColor.has256;

const icons = {
  success: chalk.green(hasIcons ? "✔" : "√"),
  error: hasIcons ? "✖" : "×",
  arrow: hasIcons ? "➔" : "-",
  bullet: hasIcons ? "•" : "*",
};

function printBenchError(suiteName, bench, error) {
  console.log(
    "\n",
    chalk.red("-"),
    chalk.yellowBright(
      `Suite ${chalk.yellow.bold.italic(suiteName)}${
        bench ? `Benchmark ${chalk.yellow.bold.italic(bench.name)}` : ""
      } failed`,
    ),
    "-",
    chalk.redBright(error),
  );
}

module.exports = {
  printSuiteName(name) {
    console.log(`${chalk.cyan(icons.bullet)} ${chalk.cyan("suite")} ${chalk.cyanBright(name)}`);
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
      console.log(`  ${chalk.gray(detail)}`);
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
        const errorKind = chalk.red.bold(bench.error ? "error" : "aborted");
        s.push(`  ${chalk.red(icons.error)} ${chalk.redBright(bench.name.padEnd(cells.name))} ${errorKind}`);
      } else {
        s.push(`  ${chalk.green(icons.success)} ${chalk.greenBright(bench.name.padEnd(cells.name))}`);
        s.push(` ${chalk.cyanBright(bench.ops.padStart(cells.ops))}${chalk.cyan(" ops/sec")}`);
        s.push(`±${bench.rme}%`.padStart(cells.rme + 3));
        s.push(`${bench.runs} runs`.padStart(cells.runs + 6));
        if (bench.fastest) {
          s.push(chalk.green(chalk.italic(" fastest")));
        } else {
          s.push(chalk.yellow(bench.diff.padStart(cells.diff + 1)));
        }
      }
      console.log(...s);
    }
    if (report.benchs.length) {
      if (report.fastest) {
        console.log(chalk.cyan(`  ${icons.arrow} Fastest is ${chalk.greenBright(report.fastest)}\n`));
      } else {
        console.log(chalk.gray(`  ${icons.arrow} No winner\n`));
      }
    } else {
      console.log(chalk.gray(`  ${icons.arrow} No benchmarks\n`));
    }
  },

  get colorFlags() {
    return chalk.supportsColor.hasBasic ? (chalk.supportsColor.has256 ? "--color=256" : "--colors") : "";
  },
};
