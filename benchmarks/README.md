Basic performance benchmark...

```
npm install fastbitset
npm install benchmark
npm install roaring

node test.js

Platform: darwin 17.6.0 x64
Intel(R) Core(TM) i7-7700K CPU @ 4.20GHz
Node version 10.4.0, v8 version 6.7.288.43-node.7


starting union query benchmark
FastBitSet (creates new bitset) x 1,101 ops/sec ±0.95% (87 runs sampled)
roaring x 2,685 ops/sec ±0.41% (95 runs sampled)
Set x 3.54 ops/sec ±0.81% (13 runs sampled)

starting intersection query benchmark
FastBitSet (creates new bitset) x 2,192 ops/sec ±0.86% (90 runs sampled)
roaring x 4,227 ops/sec ±2.73% (56 runs sampled)
Set x 5.81 ops/sec ±0.34% (19 runs sampled)
```
