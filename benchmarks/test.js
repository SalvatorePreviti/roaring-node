'use strict';

var FastBitSet = require('fastbitset');
var Benchmark = require('benchmark');
var roaring = require('roaring');
var os = require('os');

var genericSetIntersection = function(set1, set2) {
  var answer = new Set();
  if (set2.length > set1.length) {
    for (var j of set1) {
      if (set2.has(j)) {
        answer.add(j);
      }
    }
  } else {
    for (var j of set2) {
      if (set1.has(j)) {
        answer.add(j);
      }
    }
  }
  return answer;
};

var genericInplaceSetIntersection = function(set1, set2) {
  for (var j of set2) {
    if (!set1.has(j)) {
      set1.delete(j);
    }
  }
  return set1;
};

var genericSetIntersectionCard = function(set1, set2) {
  var answer = 0;
  if (set2.length > set1.length) {
    for (var j of set1) {
      if (set2.has(j)) {
        answer ++;
      }
    }
  } else {
    for (var j of set2.values()) {
      if (set1.has(j)) {
        answer ++;
      }
    }
  }
  return answer;
};

var genericSetUnion = function(set1, set2) {
  var answer = new Set(set1);
  for (var j of set2) {
    answer.add(j);
  }
  return answer;
};

var genericInplaceSetUnion = function(set1, set2) {
  for (var j of set2) {
    set1.add(j);
  }
  return set1;
};
var genericSetUnionCard = function(set1, set2) {
  return set1.size + set2.size - genericSetIntersectionCard(set1,set2);
};

var genericSetDifference = function(set1, set2) {
  var answer = new Set(set1);
  for (var j of set2) {
    answer.delete(j);
  }
  return answer;
};

var genericInplaceSetDifference = function(set1, set2) {
  for (var j of set2) {
    set1.delete(j);
  }
  return set1;
};

var genericSetDifferenceCard = function(set1, set2) {
  return set1.size - genericSetIntersectionCard(set1,set2);
};

const N = 1024 * 1024;


function AndBench() {
  console.log('starting intersection query benchmark');
  var b1 = new FastBitSet();
  var r1 = new roaring.RoaringBitmap32();
  var r2 = new roaring.RoaringBitmap32();
  var b2 = new FastBitSet();
  var s1 = new Set();
  var s2 = new Set();
  for (var i = 0 ; i < N  ; i++) {
    b1.add(3 * i + 5);
    s1.add(3 * i + 5);
    r1.add(3 * i + 5);
    r2.add(6 * i + 5);
    b2.add(6 * i + 5);
    s2.add(6 * i + 5);
  }
  var suite = new Benchmark.Suite();
  // add tests
  var ms = suite.add('FastBitSet (creates new bitset)', function() {
    return b1.new_intersection(b2);
  })
    .add('roaring', function() {
      return roaring.RoaringBitmap32.and(r1,r2);
    })
    .add('Set', function() {
      return genericSetIntersection(s1,s2);
    })
    // add listeners
    .on('cycle', function(event) {
      console.log(String(event.target));
    })
    // run async
    .run({'async': false});
}


function OrBench() {
  console.log('starting union query benchmark');
  var b1 = new FastBitSet();
  var r1 = new roaring.RoaringBitmap32();
  var r2 = new roaring.RoaringBitmap32();
  var b2 = new FastBitSet();
  var s1 = new Set();
  var s2 = new Set();
  for (var i = 0 ; i < N  ; i++) {
    b1.add(3 * i + 5);
    s1.add(3 * i + 5);
    r1.add(3 * i + 5);
    r2.add(6 * i + 5);
    b2.add(6 * i + 5);
    s2.add(6 * i + 5);
  }
  var suite = new Benchmark.Suite();
  // add tests
  var ms = suite
   .add('FastBitSet (creates new bitset)', function() {
     return b1.new_union(b2);
   })
    .add('roaring', function() {
      return roaring.RoaringBitmap32.or(r1,r2);
    })
   .add('Set', function() {
      return genericSetUnion(s1,s2);
    })
    // add listeners
    .on('cycle', function(event) {
      console.log(String(event.target));
    })
    // run async
    .run({'async': false});
}

var main = function() {
  console.log('');
  console.log('Platform: ' + process.platform + ' ' + os.release() + ' ' + process.arch);
  console.log(os.cpus()[0]['model']);
  console.log('Node version ' + process.versions.node + ', v8 version ' + process.versions.v8);
  console.log();
  console.log('');
  OrBench();
  console.log('');
  AndBench();
  console.log('');

};

if (require.main === module) {
  main();
}
