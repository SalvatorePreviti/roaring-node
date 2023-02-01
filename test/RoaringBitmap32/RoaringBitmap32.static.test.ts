import RoaringBitmap32 from "../../RoaringBitmap32";
import { expect } from "chai";

describe("RoaringBitmap32 static", () => {
  describe("constructor", () => {
    it("creates an empty bitmap with no arguments", () => {
      const bitmap = new RoaringBitmap32();
      expect(bitmap.size).eq(0);
      expect(bitmap.isEmpty).eq(true);
      expect(bitmap.isFrozen).eq(false);
    });

    it("creates an empty bitmap with an empty array", () => {
      const bitmap = new RoaringBitmap32([]);
      expect(bitmap.size).eq(0);
      expect(bitmap.isEmpty).eq(true);
      expect(bitmap.isFrozen).eq(false);
    });

    it("creates an empty bitmap with an empty bitmap", () => {
      const bitmap = new RoaringBitmap32(new RoaringBitmap32());
      expect(bitmap.size).eq(0);
      expect(bitmap.isEmpty).eq(true);
      expect(bitmap.isFrozen).eq(false);
    });

    it("construct with an array of values", () => {
      const values = [1, 2, 3, 6, 7, 8, 20, 44444];
      const bitmap = new RoaringBitmap32(values);
      bitmap.runOptimize();
      bitmap.shrinkToFit();
      expect(bitmap.size).eq(values.length);
      for (const v of values) {
        expect(bitmap.has(v)).eq(true);
      }
      expect(bitmap.has(5)).eq(false);
      expect(bitmap.isFrozen).eq(false);
    });

    it("construct with an UInt32Array", () => {
      const values = [1, 2, 3, 6, 7, 8, 20, 44444];
      const bitmap = new RoaringBitmap32(new Uint32Array(values));
      bitmap.runOptimize();
      bitmap.shrinkToFit();
      expect(bitmap.size).eq(values.length);
      for (const v of values) {
        expect(bitmap.has(v)).eq(true);
      }
      expect(bitmap.has(5)).eq(false);
      expect(bitmap.isFrozen).eq(false);
    });

    it("construct with a RoaringBitmap32", () => {
      const values = [1, 2, 3, 6, 7, 8, 20, 44444];
      const bitmap1 = new RoaringBitmap32(values);
      const bitmap2 = new RoaringBitmap32(bitmap1);
      expect(bitmap2.size).eq(values.length);
      for (const v of values) {
        expect(bitmap2.has(v)).eq(true);
      }
      expect(bitmap2.has(5)).eq(false);
      expect(bitmap1.isFrozen).eq(false);
      expect(bitmap2.isFrozen).eq(false);
    });

    it("works with an array", () => {
      const values = [100, 120, 130, 140, 150, 99, 1, 13, 64];
      const bitmap = new RoaringBitmap32(values);
      for (const value of values) {
        expect(bitmap.has(value)).eq(true);
      }
      expect(Array.from(bitmap)).deep.equal(values.slice().sort((a, b) => a - b));
      expect(bitmap.isFrozen).eq(false);
    });

    it("works with an Uint32Array", () => {
      const values = [100, 120, 130, 140, 150, 99, 1, 13, 64];
      const bitmap = new RoaringBitmap32(new Uint32Array(values));
      for (const value of values) {
        expect(bitmap.has(value)).eq(true);
      }
      expect(Array.from(bitmap)).deep.equal(values.slice().sort((a, b) => a - b));
      expect(bitmap.isFrozen).eq(false);
    });
  });

  describe("static from", () => {
    it("creates an empty bitmap with an empty array", () => {
      const bitmap = RoaringBitmap32.from([]);
      expect(bitmap.size).eq(0);
      expect(bitmap.isEmpty).eq(true);
      expect(bitmap.isFrozen).eq(false);
    });

    it("creates an empty bitmap with an empty bitmap", () => {
      const bitmap = RoaringBitmap32.from(new RoaringBitmap32());
      expect(bitmap.size).eq(0);
      expect(bitmap.isEmpty).eq(true);
      expect(bitmap.isFrozen).eq(false);
    });

    it("creates a bitmap from an array", () => {
      const bitmap = RoaringBitmap32.from([1, 3, 2, 100, 50]);
      expect(bitmap.toArray()).deep.equal([1, 2, 3, 50, 100]);
      expect(bitmap.isFrozen).eq(false);
    });

    it("creates a bitmap from an Uint32Array", () => {
      const bitmap = RoaringBitmap32.from(new Uint32Array([1, 3, 2, 100, 50]));
      expect(bitmap.toArray()).deep.equal([1, 2, 3, 50, 100]);
      expect(bitmap.isFrozen).eq(false);
    });

    it("creates a bitmap from an Int32Array", () => {
      const bitmap = RoaringBitmap32.from(new Int32Array([1, 3, 2, 100, 50]));
      expect(bitmap.toArray()).deep.equal([1, 2, 3, 50, 100]);
      expect(bitmap.isFrozen).eq(false);
    });

    it("creates a bitmap from another bitmap", () => {
      const bitmap1 = new RoaringBitmap32([1, 3, 2, 100, 50]);
      const bitmap2 = RoaringBitmap32.from(bitmap1);
      expect(bitmap1 !== bitmap2).to.be.true;
      expect(bitmap2.toArray()).deep.equal([1, 2, 3, 50, 100]);
      expect(bitmap1.isFrozen).eq(false);
      expect(bitmap2.isFrozen).eq(false);
    });
  });

  describe("static and", () => {
    it("returns empty with empty bitmaps", () => {
      const bitmap = RoaringBitmap32.and(new RoaringBitmap32(), new RoaringBitmap32());
      expect(bitmap).to.be.instanceOf(RoaringBitmap32);
      expect(bitmap.isEmpty).eq(true);
      expect(bitmap.isFrozen).eq(false);
    });

    it("returns empty with an non empty bitmap", () => {
      const bitmap = RoaringBitmap32.and(new RoaringBitmap32(), new RoaringBitmap32([1, 2, 3]));
      expect(bitmap).to.be.instanceOf(RoaringBitmap32);
      expect(bitmap.isEmpty).eq(true);
      expect(bitmap.isFrozen).eq(false);
    });

    it("ands two bitmaps", () => {
      const bitmap = RoaringBitmap32.and(new RoaringBitmap32([3, 1, 2]), new RoaringBitmap32([4, 2]));
      expect(bitmap).to.be.instanceOf(RoaringBitmap32);
      expect(bitmap.toArray()).deep.equal([2]);
      expect(bitmap.isFrozen).eq(false);
    });
  });

  describe("static or", () => {
    it("returns empty with empty bitmaps", () => {
      const bitmap = RoaringBitmap32.or(new RoaringBitmap32(), new RoaringBitmap32());
      expect(bitmap).to.be.instanceOf(RoaringBitmap32);
      expect(bitmap.isEmpty).eq(true);
      expect(bitmap.isFrozen).eq(false);
    });

    it("performs or with empty and non empty", () => {
      const c = RoaringBitmap32.or(new RoaringBitmap32(), new RoaringBitmap32([1, 3, 2]));
      expect(c.toArray()).deep.equal([1, 2, 3]);
      expect(c.isFrozen).eq(false);
    });

    it("ors two bitmaps", () => {
      const bitmap = RoaringBitmap32.or(new RoaringBitmap32([3, 1, 2]), new RoaringBitmap32([4, 2]));
      expect(bitmap.toArray()).deep.equal([1, 2, 3, 4]);
      expect(bitmap.isFrozen).eq(false);
    });
  });

  describe("static xor", () => {
    it("returns empty with empty bitmaps", () => {
      const c = RoaringBitmap32.xor(new RoaringBitmap32(), new RoaringBitmap32());
      expect(c.isEmpty).eq(true);
      expect(c.isFrozen).eq(false);
    });

    it("performs or with empty and non empty", () => {
      const bitmap = RoaringBitmap32.xor(new RoaringBitmap32(), new RoaringBitmap32([1, 2, 3]));
      expect(bitmap.toArray()).deep.equal([1, 2, 3]);
      expect(bitmap.isFrozen).eq(false);
    });

    it("xors two bitmaps", () => {
      const bitmap = RoaringBitmap32.xor(new RoaringBitmap32([3, 1, 2]), new RoaringBitmap32([4, 2]));
      expect(bitmap.toArray()).deep.equal([1, 3, 4]);
      expect(bitmap.isFrozen).eq(false);
    });
  });

  describe("static andNot", () => {
    it("returns empty with empty bitmaps", () => {
      const c = RoaringBitmap32.andNot(new RoaringBitmap32(), new RoaringBitmap32());
      expect(c.isEmpty).eq(true);
      expect(c.isFrozen).eq(false);
    });

    it("returns empty  with an non empty array", () => {
      const c = RoaringBitmap32.andNot(new RoaringBitmap32(), new RoaringBitmap32([1, 2, 3]));
      expect(c.isEmpty).eq(true);
      expect(c.isFrozen).eq(false);
    });

    it("andnots two bitmaps", () => {
      const bitmap = RoaringBitmap32.andNot(new RoaringBitmap32([3, 1, 2]), new RoaringBitmap32([4, 2]));
      expect(bitmap.toArray()).deep.equal([1, 3]);
      expect(bitmap.isFrozen).eq(false);
    });
  });

  describe("static orMany", () => {
    it("orManys multiple bitmaps (spread arguments)", () => {
      const bitmap = RoaringBitmap32.orMany(
        new RoaringBitmap32([3, 1, 2]),
        new RoaringBitmap32([1, 2, 4]),
        new RoaringBitmap32([1, 2, 6]),
      );
      expect(bitmap.toArray()).deep.equal([1, 2, 3, 4, 6]);
      expect(bitmap.isFrozen).eq(false);
    });

    it("orManys multiple bitmaps (array)", () => {
      const bitmap = RoaringBitmap32.orMany([
        new RoaringBitmap32([3, 1, 2]),
        new RoaringBitmap32([1, 2, 4]),
        new RoaringBitmap32([1, 2, 6]),
      ]);
      expect(bitmap.toArray()).deep.equal([1, 2, 3, 4, 6]);
      expect(bitmap.isFrozen).eq(false);
    });

    it("creates an empty bitmap with no argument passed", () => {
      const x = RoaringBitmap32.orMany();
      expect(x).to.be.instanceOf(RoaringBitmap32);
      expect(x.size).eq(0);
      expect(x.isEmpty).eq(true);
      expect(x.isFrozen).eq(false);
    });

    it("creates an empty bitmap with a single empty bitmap passed", () => {
      const x = RoaringBitmap32.orMany(new RoaringBitmap32());
      expect(x).to.be.instanceOf(RoaringBitmap32);
      expect(x.size).eq(0);
      expect(x.isEmpty).eq(true);
      expect(x.isFrozen).eq(false);
    });

    it("creates an empty bitmap with a single empty bitmap passed as array", () => {
      const x = RoaringBitmap32.orMany([new RoaringBitmap32()]);
      expect(x).to.be.instanceOf(RoaringBitmap32);
      expect(x.size).eq(0);
      expect(x.isEmpty).eq(true);
      expect(x.isFrozen).eq(false);
    });

    it("creates an empty bitmap with multiple empty bitmap passed", () => {
      const x = RoaringBitmap32.orMany(new RoaringBitmap32(), new RoaringBitmap32(), new RoaringBitmap32());
      expect(x).to.be.instanceOf(RoaringBitmap32);
      expect(x.size).eq(0);
      expect(x.isEmpty).eq(true);
      expect(x.isFrozen).eq(false);
    });

    it("creates an empty bitmap with a multiple empty bitmap passed as array", () => {
      const x = RoaringBitmap32.orMany([new RoaringBitmap32(), new RoaringBitmap32(), new RoaringBitmap32()]);
      expect(x).to.be.instanceOf(RoaringBitmap32);
      expect(x.size).eq(0);
      expect(x.isEmpty).eq(true);
      expect(x.isFrozen).eq(false);
    });
  });

  describe("static xorMany", () => {
    it("xorManys multiple bitmaps (spread arguments)", () => {
      const bitmap = RoaringBitmap32.xorMany(
        new RoaringBitmap32([3, 1, 2]),
        new RoaringBitmap32([3, 2]),
        new RoaringBitmap32([6, 7, 8]),
        new RoaringBitmap32([7, 4]),
      );
      expect(bitmap.toArray()).deep.equal([1, 4, 6, 8]);
      expect(bitmap.isFrozen).eq(false);
    });

    it("xorManys multiple bitmaps (array)", () => {
      const bitmap = RoaringBitmap32.xorMany([
        new RoaringBitmap32([3, 1, 2]),
        new RoaringBitmap32([3, 2]),
        new RoaringBitmap32([6, 7, 8]),
        new RoaringBitmap32([7, 4]),
      ]);
      expect(bitmap.toArray()).deep.equal([1, 4, 6, 8]);
      expect(bitmap.isFrozen).eq(false);
    });

    it("creates an empty bitmap with no argument passed", () => {
      const x = RoaringBitmap32.xorMany();
      expect(x).to.be.instanceOf(RoaringBitmap32);
      expect(x.size).eq(0);
      expect(x.isEmpty).eq(true);
      expect(x.isFrozen).eq(false);
    });

    it("creates an empty bitmap with a single empty bitmap passed", () => {
      const x = RoaringBitmap32.xorMany(new RoaringBitmap32());
      expect(x).to.be.instanceOf(RoaringBitmap32);
      expect(x.size).eq(0);
      expect(x.isEmpty).eq(true);
      expect(x.isFrozen).eq(false);
    });

    it("creates an empty bitmap with a single empty bitmap passed as array", () => {
      const x = RoaringBitmap32.xorMany([new RoaringBitmap32()]);
      expect(x).to.be.instanceOf(RoaringBitmap32);
      expect(x.size).eq(0);
      expect(x.isEmpty).eq(true);
      expect(x.isFrozen).eq(false);
    });

    it("creates an empty bitmap with multiple empty bitmap passed", () => {
      const x = RoaringBitmap32.xorMany(new RoaringBitmap32(), new RoaringBitmap32(), new RoaringBitmap32());
      expect(x).to.be.instanceOf(RoaringBitmap32);
      expect(x.size).eq(0);
      expect(x.isEmpty).eq(true);
      expect(x.isFrozen).eq(false);
    });

    it("creates an empty bitmap with a multiple empty bitmap passed as array", () => {
      const x = RoaringBitmap32.xorMany([new RoaringBitmap32(), new RoaringBitmap32(), new RoaringBitmap32()]);
      expect(x).to.be.instanceOf(RoaringBitmap32);
      expect(x.size).eq(0);
      expect(x.isEmpty).eq(true);
      expect(x.isFrozen).eq(false);
    });
  });

  describe("static swap", () => {
    it("swaps two empty bitmaps", () => {
      const a = new RoaringBitmap32();
      const b = new RoaringBitmap32();
      RoaringBitmap32.swap(a, b);
      expect(a.size).eq(0);
      expect(a.isEmpty).eq(true);
      expect(b.size).eq(0);
      expect(b.isEmpty).eq(true);
      expect(a.isFrozen).eq(false);
      expect(b.isFrozen).eq(false);
    });

    it("swaps one empty with a non empty bitmap", () => {
      const a = new RoaringBitmap32();
      const b = new RoaringBitmap32([1, 2, 3]);
      RoaringBitmap32.swap(a, b);
      expect(a.size).eq(3);
      expect(a.isEmpty).eq(false);
      expect(b.size).eq(0);
      expect(b.isEmpty).eq(true);
      expect(a.isFrozen).eq(false);
      expect(b.isFrozen).eq(false);
    });

    it("swaps two bitmaps", () => {
      const a = new RoaringBitmap32([4, 5]);
      const b = new RoaringBitmap32([1, 2, 3]);
      RoaringBitmap32.swap(a, b);
      expect(a.size).eq(3);
      expect(a.isEmpty).eq(false);
      expect(b.size).eq(2);
      expect(b.isEmpty).eq(false);
      expect(a.toArray()).deep.equal([1, 2, 3]);
      expect(b.toArray()).deep.equal([4, 5]);
      expect(a.isFrozen).eq(false);
      expect(b.isFrozen).eq(false);
    });
  });
});
