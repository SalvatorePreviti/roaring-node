try {
  process.env.npm_config_node_gyp = require.resolve("node-gyp/bin/node-gyp.js");
} catch (_e) {}

require("@mapbox/node-pre-gyp/lib/main");