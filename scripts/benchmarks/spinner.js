const chalk = require("chalk");
const readline = require("readline");

const spinner = {
  interval: null,
  printed: false,
  start() {
    if (!spinner.interval && chalk.supportsColor.hasBasic && !process.env.CI) {
      const dot = chalk.gray(".");
      spinner.interval =
        chalk.supportsColor.hasBasic &&
        setInterval(() => {
          spinner.printed = true;
          process.stderr.write(dot);
        }, 1000);
    }
  },
  clear() {
    if (spinner.printed) {
      spinner.printed = false;
      readline.clearLine(process.stderr);
      readline.cursorTo(process.stderr, 0);
    }
  },
  stop() {
    if (spinner.interval) {
      spinner.clear();
      clearInterval(spinner.interval);
      spinner.interval = null;
    }
  },
};

module.exports = spinner;
