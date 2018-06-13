delete process.env.ROARING_DISABLE_SSE42
delete process.env.ROARING_DISABLE_AVX2
process.env.ROARING_TEST_EXPECTED_CPU = 'AVX2'
