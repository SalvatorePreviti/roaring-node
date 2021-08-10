module.exports = {
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
      tsConfig: './tsconfig.test.json'
    }
  }
}
