// lint-staged.config.js

module.exports = {
  "*": () => {
    return [`biome check --write`, "tsc --noEmit"];
  },
};
