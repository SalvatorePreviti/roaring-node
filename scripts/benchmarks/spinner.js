const colors = require("ansis");
const readline = require("readline");

const spinner = {
  interval: null,
  printed: false,
  start() {
    if (!spinner.interval && !process.env.CI) {
      const dot = colors.gray(".");
      spinner.interval = setInterval(() => {
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
