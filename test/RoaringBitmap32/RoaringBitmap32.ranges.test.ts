import { expect } from "chai";
import RoaringBitmap32 from "../../RoaringBitmap32";
import RoaringBitmap32Iterator from "../../RoaringBitmap32Iterator";

describe("RoaringBitmap32 ranges", () => {
  describe("fromRange", () => {
    it("returns an empty bitmap with invalid values", () => {
      expect(RoaringBitmap32.fromRange(-1, -1).isEmpty).eq(true);
      expect(RoaringBitmap32.fromRange(89999999999, 99999999999).isEmpty).eq(true);
      expect(RoaringBitmap32.fromRange(0, 0).isEmpty).eq(true);
      expect(RoaringBitmap32.fromRange(20, 20).isEmpty).eq(true);
      expect(RoaringBitmap32.fromRange(null as any, 20).isEmpty).eq(true);
    });

    it("works with range 0, 6", () => {
      const bitmap = RoaringBitmap32.fromRange(0, 6);
      expect(bitmap.size).eq(6);
      expect(bitmap.toArray()).deep.equal([0, 1, 2, 3, 4, 5]);
    });

    it("works with range 10, 16", () => {
      const bitmap = RoaringBitmap32.fromRange(10, 16);
      expect(bitmap.size).eq(6);
      expect(bitmap.toArray()).deep.equal([10, 11, 12, 13, 14, 15]);
    });

    it("works with range 100, 70000", () => {
      const bitmap = RoaringBitmap32.fromRange(100, 70000);
      expect(bitmap.size).eq(69900);
    });

    it("works with step 2", () => {
      const bitmap = RoaringBitmap32.fromRange(0, 20, 2);
      expect(bitmap.toArray()).deep.equal([0, 2, 4, 6, 8, 10, 12, 14, 16, 18]);
    });

    it("works with step 5", () => {
      const bitmap = RoaringBitmap32.fromRange(0, 53, 5);
      expect(bitmap.toArray()).deep.equal([0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50]);
    });

    it("works with big values", () => {
      expect(RoaringBitmap32.fromRange(0xfffffffd, 0xfffffffe).toArray()).deep.equal([0xfffffffd]);
      expect(RoaringBitmap32.fromRange(0xfffffffd, 0xffffffff).toArray()).deep.equal([0xfffffffd, 0xfffffffe]);
      expect(RoaringBitmap32.fromRange(0xfffffffe, 0x100000000).toArray()).deep.equal([0xfffffffe, 0xffffffff]);
      expect(RoaringBitmap32.fromRange(0xfffffffe, Number.MAX_SAFE_INTEGER).toArray()).deep.equal([
        0xfffffffe, 0xffffffff,
      ]);
      expect(RoaringBitmap32.fromRange(0xffffffff, 0xffffffff).toArray()).deep.equal([]);
      expect(RoaringBitmap32.fromRange(0xffffffff, 0x100000000).toArray()).deep.equal([0xffffffff]);
      expect(RoaringBitmap32.fromRange(0x100000000, 0x100000005).toArray()).deep.equal([]);
    });

    it("works with negative values", () => {
      expect(RoaringBitmap32.fromRange(-1, 3).toArray()).deep.equal([0, 1, 2]);
      expect(RoaringBitmap32.fromRange(-1000, 3).toArray()).deep.equal([0, 1, 2]);
      expect(RoaringBitmap32.fromRange(Number.MIN_SAFE_INTEGER, 3).toArray()).deep.equal([0, 1, 2]);
    });
  });

  describe("hasRange", () => {
    it("returns false for invalid values", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5]);
      expect(bitmap.hasRange(null as any, 6)).eq(false);
      expect(bitmap.hasRange("0" as any, 6)).eq(false);
      expect(bitmap.hasRange(0, [8] as any)).eq(false);
      expect(bitmap.hasRange(NaN, 4)).eq(false);
      expect(bitmap.hasRange(4, 4)).eq(false);
      expect(bitmap.hasRange(-2, -4)).eq(false);
      expect(bitmap.hasRange(-4, -2)).eq(false);
      expect(bitmap.hasRange(4, 2)).eq(false);
      expect(bitmap.hasRange(4, -2)).eq(false);
    });

    it("returns false for an empty bitmap", () => {
      const bitmap = new RoaringBitmap32();
      expect(bitmap.hasRange(0, 3)).eq(false);
      expect(bitmap.hasRange(0, 0xffffffff)).eq(false);
      expect(bitmap.hasRange(0, 0xffffffff + 1)).eq(false);
      expect(bitmap.hasRange(0xfffffffe, 0xffffffff + 1)).eq(false);
      expect(bitmap.hasRange(0xffffffff, 0xffffffff + 1)).eq(false);
    });

    it("returns false for areas that contains no values", () => {
      const bitmap = new RoaringBitmap32([100, 101, 102, 103, 105]);
      expect(bitmap.hasRange(99, 101)).eq(false);
      expect(bitmap.hasRange(105, 107)).eq(false);
      expect(bitmap.hasRange(105, 109)).eq(false);
      expect(bitmap.hasRange(100, 106)).eq(false);
    });

    it("works with simple values", () => {
      const bitmap = new RoaringBitmap32([100, 101, 102, 103, 105]);
      expect(bitmap.hasRange(100, 102)).eq(true);
      expect(bitmap.hasRange(100, 103)).eq(true);
      expect(bitmap.hasRange(100, 104)).eq(true);
      expect(bitmap.hasRange(100, 105)).eq(false);
    });

    it("accepts floating point values", () => {
      const bitmap = new RoaringBitmap32([50, 51, 52, 53, 55]);

      expect(bitmap.hasRange(50, 52)).eq(true);
      expect(bitmap.hasRange(50, 53)).eq(true);
      expect(bitmap.hasRange(50, 54)).eq(true);
      expect(bitmap.hasRange(50, 55)).eq(false);

      expect(bitmap.hasRange(50, 52.1)).eq(true);
      expect(bitmap.hasRange(50, 52.6)).eq(true);
      expect(bitmap.hasRange(50, 53.1)).eq(true);
      expect(bitmap.hasRange(50, 53.6)).eq(true);
      expect(bitmap.hasRange(50, 54.1)).eq(false);
      expect(bitmap.hasRange(50, 54.6)).eq(false);
      expect(bitmap.hasRange(50.3, 52.4)).eq(true);
      expect(bitmap.hasRange(50.7, 52.7)).eq(true);

      expect(bitmap.hasRange(49.9, 52)).eq(true);
      expect(bitmap.hasRange(49.9, 52.2)).eq(true);
      expect(bitmap.hasRange(49.9, 52.6)).eq(true);
      expect(bitmap.hasRange(49.9, 53.6)).eq(true);
      expect(bitmap.hasRange(49.9, 54.6)).eq(false);

      expect(bitmap.hasRange(55, 55.2)).eq(true);
      expect(bitmap.hasRange(55, 55.5)).eq(true);
      expect(bitmap.hasRange(55, 55.7)).eq(true);
    });

    it("works with 0xFFFFFFFF", () => {
      const bitmap = new RoaringBitmap32([4294967295]);
      expect(bitmap.hasRange(4294967295, 4294967296)).eq(true);
      expect(bitmap.hasRange(4294967294, 4294967296)).eq(false);
      expect(bitmap.hasRange(4294967295, 4294967297)).eq(false);
      expect(bitmap.hasRange(4294967294, 4294967297)).eq(false);
    });

    it("works with 0xFFFFFFFE and 0xFFFFFFFF", () => {
      const bitmap = new RoaringBitmap32([4294967294, 4294967295]);
      expect(bitmap.has(4294967293)).eq(false);
      expect(bitmap.has(4294967294)).eq(true);
      expect(bitmap.has(4294967295)).eq(true);
      expect(bitmap.hasRange(4294967295, 4294967296)).eq(true);
      expect(bitmap.hasRange(4294967294, 4294967295)).eq(true);
      expect(bitmap.hasRange(4294967293, 4294967294)).eq(false);
      expect(bitmap.hasRange(4294967295, 4294967296)).eq(true);
      expect(bitmap.hasRange(4294967294, 4294967297)).eq(false);
      expect(bitmap.hasRange(4294967293, 4294967296)).eq(false);
      expect(bitmap.hasRange(4294967293, 4294967297)).eq(false);
    });

    it("works with 0xFFFFFFFD, 0xFFFFFFFE and 0xFFFFFFFF", () => {
      const bitmap = new RoaringBitmap32([4294967293, 4294967294, 4294967295]);
      expect(bitmap.has(4294967292)).eq(false);
      expect(bitmap.has(4294967293)).eq(true);
      expect(bitmap.has(4294967294)).eq(true);
      expect(bitmap.has(4294967295)).eq(true);

      expect(bitmap.hasRange(4294967292, 4294967293)).eq(false);
      expect(bitmap.hasRange(4294967292, 4294967294)).eq(false);
      expect(bitmap.hasRange(4294967292, 4294967295)).eq(false);
      expect(bitmap.hasRange(4294967292, 4294967296)).eq(false);

      expect(bitmap.hasRange(4294967293, 4294967297)).eq(false);
      expect(bitmap.hasRange(4294967294, 4294967297)).eq(false);
      expect(bitmap.hasRange(4294967295, 4294967297)).eq(false);
      expect(bitmap.hasRange(4294967296, 4294967297)).eq(false);

      expect(bitmap.hasRange(4294967293, 4294967294)).eq(true);
      expect(bitmap.hasRange(4294967293, 4294967295)).eq(true);
      expect(bitmap.hasRange(4294967293, 4294967296)).eq(true);

      expect(bitmap.hasRange(4294967294, 4294967295)).eq(true);
      expect(bitmap.hasRange(4294967294, 4294967296)).eq(true);

      expect(bitmap.hasRange(4294967295, 4294967296)).eq(true);
    });
  });

  describe("rangeCardinality", () => {
    it("returns the valid size for the basic test", () => {
      const s = 65536;
      const r = new RoaringBitmap32();
      r.addRange(s * 2, s * 10);

      // single container (minhb == maxhb)
      expect(r.rangeCardinality(s * 2, s * 3)).eq(s);
      expect(r.rangeCardinality(s * 2 + 100, s * 3)).eq(s - 100);
      expect(r.rangeCardinality(s * 2, s * 3 - 200)).eq(s - 200);
      expect(r.rangeCardinality(s * 2 + 100, s * 3 - 200)).eq(s - 300);
      // multiple containers (maxhb > minhb)
      expect(r.rangeCardinality(s * 2, s * 5)).eq(s * 3);
      expect(r.rangeCardinality(s * 2 + 100, s * 5)).eq(s * 3 - 100);
      expect(r.rangeCardinality(s * 2, s * 5 - 200)).eq(s * 3 - 200);
      expect(r.rangeCardinality(s * 2 + 100, s * 5 - 200)).eq(s * 3 - 300);
      // boundary checks
      expect(r.rangeCardinality(s * 20, s * 21)).eq(0);
      expect(r.rangeCardinality(100, 100)).eq(0);
      expect(r.rangeCardinality(0, s * 7)).eq(s * 5);
      expect(r.rangeCardinality(s * 7, Number.MAX_SAFE_INTEGER)).eq(s * 3);
      // Extremes
      expect(r.rangeCardinality(s * 7, Number.POSITIVE_INFINITY)).eq(s * 3);
      expect(r.rangeCardinality(-1, Number.POSITIVE_INFINITY)).eq(r.size);
      expect(r.rangeCardinality(Number.NEGATIVE_INFINITY, Number.POSITIVE_INFINITY)).eq(r.size);
    });
  });

  describe("addRange", () => {
    it("does nothing for invalid values", () => {
      const bitmap = new RoaringBitmap32([1, 2]);
      expect(bitmap.toArray()).deep.equal([1, 2]);
      bitmap.addRange(null as any, 6);
      bitmap.addRange("0" as any, 6);
      bitmap.addRange(0, [8] as any);
      // bitmap.addRange(NaN, 4)
      bitmap.addRange(4, 4);
      bitmap.addRange(-2, -4);
      bitmap.addRange(-4, -2);
      bitmap.addRange(4, 2);
      bitmap.addRange(4, -2);
      expect(bitmap.toArray()).deep.equal([1, 2]);
    });

    it("flips a ranges on an empty bitmap", () => {
      const bitmap = new RoaringBitmap32();
      bitmap.addRange(1, 3);
      bitmap.addRange(3, 5);
      bitmap.addRange(8, 10);
      expect(bitmap.toArray()).deep.equal([1, 2, 3, 4, 8, 9]);
    });

    it("accepts negative numbers", () => {
      const bitmap = new RoaringBitmap32([1]);
      bitmap.addRange(-4, 3);
      expect(bitmap.toArray()).deep.equal([0, 1, 2]);
    });

    it("accepts negative infinity", () => {
      const bitmap = new RoaringBitmap32([1]);
      bitmap.addRange(-Infinity, 3);
      expect(bitmap.toArray()).deep.equal([0, 1, 2]);
    });

    it("adds ranges", () => {
      const bitmap = new RoaringBitmap32([4, 6, 7]);
      bitmap.addRange(1, 3);
      bitmap.addRange(3, 5);
      expect(bitmap.toArray()).deep.equal([1, 2, 3, 4, 6, 7]);
      bitmap.addRange(8, 10);
      bitmap.addRange(4, 10);
      expect(bitmap.toArray()).deep.equal([1, 2, 3, 4, 5, 6, 7, 8, 9]);
      bitmap.addRange(4, 10);
      expect(bitmap.toArray()).deep.equal([1, 2, 3, 4, 5, 6, 7, 8, 9]);
    });

    it("adds 0xFFFFFFFF", () => {
      const bitmap = new RoaringBitmap32();
      bitmap.addRange(4294967295, 4294967296);
      expect(bitmap.has(4294967294)).eq(false);
      expect(bitmap.has(4294967295)).eq(true);
    });

    it("adds 0xFFFFFFFE and 0xFFFFFFFF", () => {
      const bitmap = new RoaringBitmap32();
      bitmap.addRange(4294967294, 4294967296);
      expect(bitmap.has(4294967293)).eq(false);
      expect(bitmap.has(4294967294)).eq(true);
      expect(bitmap.has(4294967295)).eq(true);
    });

    it("supports very big numbers", () => {
      const bitmap = new RoaringBitmap32();
      bitmap.addRange(4294967294, 9000000000);
      expect(bitmap.has(4294967294)).eq(true);
      expect(bitmap.has(4294967295)).eq(true);
    });

    it("supports positive infinity", () => {
      const bitmap = new RoaringBitmap32();
      bitmap.addRange(4294967294, Infinity);
      expect(bitmap.has(4294967294)).eq(true);
      expect(bitmap.has(4294967295)).eq(true);
    });

    it("enumerates correctly when adding a range the end", () => {
      const bitmap = new RoaringBitmap32();
      const iterator = new RoaringBitmap32Iterator(bitmap);
      bitmap.addRange(4294967295, 4294967296);
      expect(iterator.next()).deep.equal({ done: false, value: 4294967295 });
      expect(iterator.next()).deep.equal({ done: true, value: undefined });
    });
  });

  describe("removeRange", () => {
    it("does nothing for invalid values", () => {
      const values = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
      const bitmap = new RoaringBitmap32(values);
      bitmap.removeRange(null as any, 10);
      bitmap.removeRange(-19, -10);
      bitmap.removeRange(-10, -19);
      bitmap.removeRange(10, 3);
      bitmap.removeRange(0, 0);
      bitmap.removeRange(5, 5);
      bitmap.removeRange(5, "xxx" as any);
      expect(bitmap.toArray()).deep.equal(values);
    });

    it("removes a range", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5, 6, 7, 8, 9, 10]);
      bitmap.removeRange(3, 7);
      expect(bitmap.toArray()).deep.equal([1, 2, 7, 8, 9, 10]);
    });
  });

  describe("flipRange", () => {
    it("does nothing for invalid values", () => {
      const bitmap = new RoaringBitmap32([1, 2]);
      expect(bitmap.toArray()).deep.equal([1, 2]);
      bitmap.flipRange(null as any, 6);
      bitmap.flipRange("0" as any, 6);
      bitmap.flipRange(0, [8] as any);
      bitmap.flipRange(4, 4);
      bitmap.flipRange(-2, -4);
      bitmap.flipRange(-4, -2);
      bitmap.flipRange(4, 2);
      bitmap.flipRange(4, -2);
      expect(bitmap.toArray()).deep.equal([1, 2]);
    });

    it("flips a ranges on an empty bitmap", () => {
      const bitmap = new RoaringBitmap32();
      expect(bitmap.flipRange(1, 3)).eq(bitmap);
      bitmap.flipRange(3, 5);
      bitmap.flipRange(8, 10);
      expect(bitmap.toArray()).deep.equal([1, 2, 3, 4, 8, 9]);
    });

    it("accepts negative numbers", () => {
      const bitmap = new RoaringBitmap32([1]);
      bitmap.flipRange(-4, 3);
      expect(bitmap.toArray()).deep.equal([0, 2]);
    });

    it("accepts negative infinity", () => {
      const bitmap = new RoaringBitmap32([1]);
      bitmap.flipRange(-Infinity, 3);
      expect(bitmap.toArray()).deep.equal([0, 2]);
    });

    it("flips a ranges", () => {
      const bitmap = new RoaringBitmap32([4, 6, 7]);
      bitmap.flipRange(1, 3);
      bitmap.flipRange(3, 5);
      bitmap.flipRange(8, 10);
      bitmap.flipRange(4, 10);
      expect(bitmap.toArray()).deep.equal([1, 2, 3, 4, 5]);
      bitmap.flipRange(4, 10);
      expect(bitmap.toArray()).deep.equal([1, 2, 3, 6, 7, 8, 9]);
    });

    it("flips 0xFFFFFFFF", () => {
      const bitmap = new RoaringBitmap32();
      bitmap.flipRange(4294967295, 4294967296);
      expect(bitmap.has(4294967294)).eq(false);
      expect(bitmap.has(4294967295)).eq(true);
    });

    it("flips 0xFFFFFFFE and 0xFFFFFFFF", () => {
      const bitmap = new RoaringBitmap32();
      bitmap.flipRange(4294967294, 4294967296);
      expect(bitmap.has(4294967293)).eq(false);
      expect(bitmap.has(4294967294)).eq(true);
      expect(bitmap.has(4294967295)).eq(true);
    });

    it("supports very big numbers", () => {
      const bitmap = new RoaringBitmap32();
      bitmap.flipRange(4294967294, 9000000000);
      expect(bitmap.has(4294967294)).eq(true);
      expect(bitmap.has(4294967295)).eq(true);
    });

    it("supports positive infinity", () => {
      const bitmap = new RoaringBitmap32();
      bitmap.flipRange(4294967294, Infinity);
      expect(bitmap.has(4294967294)).eq(true);
      expect(bitmap.has(4294967295)).eq(true);
    });

    it("enumerates correctly when flipping a range at the end", () => {
      const bitmap = new RoaringBitmap32();
      const iterator = new RoaringBitmap32Iterator(bitmap);
      bitmap.flipRange(4294967295, 4294967296);
      expect(iterator.next()).deep.equal({ done: false, value: 4294967295 });
      expect(iterator.next()).deep.equal({ done: true, value: undefined });
    });
  });
});
