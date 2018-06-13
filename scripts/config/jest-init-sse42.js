delete process.env.ROARING_DISABLE_SSE42
process.env.ROARING_DISABLE_AVX2 = 'true'
process.env.ROARING_TEST_EXPECTED_CPU = 'SSE42'
