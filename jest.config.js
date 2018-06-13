const cpuinfo = require('./lib/cpuinfo')

function makeTestProject() {
  return {
    testEnvironment: 'node',
    verbose: true,
    transform: {
      '^.+\\.tsx?$': 'ts-jest'
    },
    testPathIgnorePatterns: ['/node_modules/', '/temp/', '/src/'],
    testRegex: '(/__tests__/.*|\\.(test|spec))\\.(jsx?|tsx?)$',
    moduleFileExtensions: ['ts', 'tsx', 'js', 'jsx', 'json', 'node'],
    globals: {
      'ts-jest': {
        tsConfigFile: './test/tsconfig.json'
      }
    }
  }
}

const defaultProject = makeTestProject()

defaultProject.setupFiles = ['./scripts/config/jest-init-plain.js']

const projects = [defaultProject]

if (cpuinfo.AVX2) {
  const avx2Project = makeTestProject()
  avx2Project.displayName = 'AVX'
  avx2Project.setupFiles = ['./scripts/config/jest-init-avx2.js']
  projects.push(avx2Project)
}

if (cpuinfo.SSE42) {
  const sse42Project = makeTestProject()
  sse42Project.displayName = 'SSE'
  sse42Project.setupFiles = ['./scripts/config/jest-init-sse4.js']
  projects.push(sse42Project)
}

if (projects.length === 1) {
  module.exports = defaultProject
} else {
  defaultProject.displayName = ' C '
  module.exports = {
    projects
  }
}
