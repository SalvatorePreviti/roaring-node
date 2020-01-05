#!/usr/bin/env node

const args = []
args.push(process.argv[0])
args.push('-t', '8.0.0')
args.push('-t', '9.0.0')
args.push('-t', '10.0.0')
args.push('-t', '11.0.0')
args.push('-t', '12.0.0')
args.push('-t', '13.0.0')

if (process.env.PREBUILD_GITHUB_TOKEN) {
  args.push('-u', process.env.PREBUILD_GITHUB_TOKEN)
}

for (let i = 2; i < process.argv.length; ++i) {
  args.push(process.argv[i])
}

const oldProcessArgv = process.argv

process.argv = args

try {
  require('prebuild/bin')
} finally {
  process.argv = oldProcessArgv
}
