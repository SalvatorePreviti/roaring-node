function testProject() {
  return {
    testEnvironment: 'node',
    verbose: true,
    /* transform: {
      '^.+\\.tsx?$': 'ts-jest'
    }, */
    testPathIgnorePatterns: ['/node_modules/', '/temp/', '/src/'],
    testRegex: '(/__tests__/.*|\\.(test|spec))\\.(jsx?|tsx?)$',
    moduleFileExtensions: ['ts', 'tsx', 'js', 'jsx', 'json', 'node'],
    globals: {
      'ts-jest': {
        tsConfig: './tsconfig.test.json'
      }
    }
  }
}

const defaultProject = testProject()

defaultProject.setupFiles = ['./test/config/jest-init-plain.js']

const projects = [defaultProject]

/* 
const instructionSet = require('./lib/instructionSet')
if (instructionSet === 'AVX2') {
  const avx2Project = testProject()
  avx2Project.displayName = 'AVX2 '
  avx2Project.setupFiles = ['./test/config/jest-init-avx2.js']
  projects.push(avx2Project)
}

if (instructionSet === 'AVX2' || instructionSet === 'SSE42') {
  const sse42Project = testProject()
  sse42Project.displayName = 'SSE42'
  sse42Project.setupFiles = ['./test/config/jest-init-sse4.js']
  projects.push(sse42Project)
} */

if (projects.length === 1) {
  module.exports = defaultProject
} else {
  defaultProject.displayName = 'PLAIN'

  module.exports = {
    projects
  }
}
