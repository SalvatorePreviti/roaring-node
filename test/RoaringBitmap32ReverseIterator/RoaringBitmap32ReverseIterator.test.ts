import { describe, expect, it } from "vitest";
import RoaringBitmap32 from "../../RoaringBitmap32";
import RoaringBitmap32ReverseIterator from "../../RoaringBitmap32ReverseIterator";

describe("RoaringBitmap32ReverseIterator", () => {
  describe("constructor", () => {
    it("is a class", () => {
      expect(typeof RoaringBitmap32).eq("function");
    });

    it("creates an empty iterator with no arguments", () => {
      const iter = new RoaringBitmap32ReverseIterator();
      expect(iter).to.be.instanceOf(RoaringBitmap32ReverseIterator);
    });

    it("creates an iterator with a RoaringBitmap32", () => {
      const bitmap = new RoaringBitmap32([3, 4, 5]);
      const iter = new RoaringBitmap32ReverseIterator(bitmap);
      expect(iter).to.be.instanceOf(RoaringBitmap32ReverseIterator);
    });

    it("throws an exception if called with a non RoaringBitmap32", () => {
      expect(() => new RoaringBitmap32ReverseIterator(123 as any)).to.throw(Error);
      expect(() => new RoaringBitmap32ReverseIterator([123] as any)).to.throw(Error);
    });
  });

  describe("next", () => {
    it("is a function", () => {
      const iter = new RoaringBitmap32ReverseIterator();
      expect(typeof iter.next).eq("function");
    });

    it("returns an empty result if iterator is created without arguments", () => {
      const iter = new RoaringBitmap32ReverseIterator();
      expect(iter.next()).deep.equal({ value: undefined, done: true });
      expect(iter.next()).deep.equal({ value: undefined, done: true });
    });

    it("returns an empty result if iterator is created with an empty RoaringBitmap32", () => {
      const iter = new RoaringBitmap32ReverseIterator(new RoaringBitmap32());
      expect(iter.next()).deep.equal({ value: undefined, done: true });
      expect(iter.next()).deep.equal({ value: undefined, done: true });
    });

    it("allows iterating a small array", () => {
      const iter = new RoaringBitmap32ReverseIterator(new RoaringBitmap32([123, 456, 999, 1000]));
      expect(iter.next()).deep.equal({ value: 1000, done: false });
      expect(iter.next()).deep.equal({ value: 999, done: false });
      expect(iter.next()).deep.equal({ value: 456, done: false });
      expect(iter.next()).deep.equal({ value: 123, done: false });
      expect(iter.next()).deep.equal({ value: undefined, done: true });
      expect(iter.next()).deep.equal({ value: undefined, done: true });
      expect(iter.next()).deep.equal({ value: undefined, done: true });
    });
  });

  describe("Symbol.iterator", () => {
    it("is a function", () => {
      const iter = new RoaringBitmap32ReverseIterator();
      expect(typeof iter[Symbol.iterator]).deep.equal("function");
    });

    it("returns this", () => {
      const iter = new RoaringBitmap32ReverseIterator();
      expect(iter[Symbol.iterator]()).eq(iter);
    });

    it("allows foreach (empty)", () => {
      const iter = new RoaringBitmap32ReverseIterator();
      expect(iter.next()).deep.equal({ done: true, value: undefined });
    });

    it("allows foreach (small array)", () => {
      const iter = new RoaringBitmap32ReverseIterator(new RoaringBitmap32([123, 456, 789]));
      const values = [];
      for (const x of iter) {
        values.push(x);
      }
      expect(values.reverse()).deep.equal([123, 456, 789]);
    });

    it("allows Array.from", () => {
      const iter = new RoaringBitmap32ReverseIterator(new RoaringBitmap32([123, 456, 789]));
      const values = Array.from(iter);
      expect(values.reverse()).deep.equal([123, 456, 789]);
    });
  });

  describe("buffer (number)", () => {
    it("iterates, buffer 1, bitmap 0", () => {
      const bitmap = new RoaringBitmap32();
      const iterator = new RoaringBitmap32ReverseIterator(bitmap, 1);
      expect(iterator.next()).deep.equal({ value: undefined, done: true });
      expect(iterator.next()).deep.equal({ value: undefined, done: true });
    });

    it("iterates, buffer 2, bitmap 0", () => {
      const bitmap = new RoaringBitmap32();
      const iterator = new RoaringBitmap32ReverseIterator(bitmap, 2);
      expect(iterator.next()).deep.equal({ value: undefined, done: true });
      expect(iterator.next()).deep.equal({ value: undefined, done: true });
    });

    it("iterates, buffer 1, bitmap 1", () => {
      const bitmap = new RoaringBitmap32([5]);
      const iterator = new RoaringBitmap32ReverseIterator(bitmap, 1);
      expect(iterator.next()).deep.equal({ value: 5, done: false });
      expect(iterator.next()).deep.equal({ value: undefined, done: true });
      expect(iterator.next()).deep.equal({ value: undefined, done: true });
    });

    it("iterates, buffer 2, bitmap 1", () => {
      const bitmap = new RoaringBitmap32([5]);
      const iterator = new RoaringBitmap32ReverseIterator(bitmap, 2);
      expect(iterator.next()).deep.equal({ value: 5, done: false });
      expect(iterator.next()).deep.equal({ value: undefined, done: true });
      expect(iterator.next()).deep.equal({ value: undefined, done: true });
    });

    it("iterates, buffer 1, bitmap 3", () => {
      const bitmap = new RoaringBitmap32([5, 7, 9]);
      const iterator = new RoaringBitmap32ReverseIterator(bitmap, 1);
      expect(iterator.next()).deep.equal({ value: 9, done: false });
      expect(iterator.next()).deep.equal({ value: 7, done: false });
      expect(iterator.next()).deep.equal({ value: 5, done: false });
    });

    it("iterates, buffer 2, bitmap 3", () => {
      const bitmap = new RoaringBitmap32([5, 7, 9]);
      const iterator = new RoaringBitmap32ReverseIterator(bitmap, 2);
      expect(iterator.next()).deep.equal({ value: 9, done: false });
      expect(iterator.next()).deep.equal({ value: 7, done: false });
      expect(iterator.next()).deep.equal({ value: 5, done: false });
    });
  });

  describe("buffer (Uint32Array)", () => {
    it("iterates, buffer 1, bitmap 0", () => {
      const bitmap = new RoaringBitmap32();
      const buffer = new Uint32Array(1);
      const iterator = new RoaringBitmap32ReverseIterator(bitmap, buffer);
      expect(iterator.next()).deep.equal({ value: undefined, done: true });
      expect(iterator.next()).deep.equal({ value: undefined, done: true });
      expect(Array.from(buffer)).deep.equal([0]);
    });

    it("iterates, buffer 2, bitmap 0", () => {
      const bitmap = new RoaringBitmap32();
      const buffer = new Uint32Array(2);
      const iterator = new RoaringBitmap32ReverseIterator(bitmap, buffer);
      expect(iterator.next()).deep.equal({ value: undefined, done: true });
      expect(iterator.next()).deep.equal({ value: undefined, done: true });
      expect(Array.from(buffer)).deep.equal([0, 0]);
    });

    it("iterates, buffer 1, bitmap 1", () => {
      const bitmap = new RoaringBitmap32([5]);
      const buffer = new Uint32Array(1);
      const iterator = new RoaringBitmap32ReverseIterator(bitmap, buffer);
      expect(buffer[0]).eq(0);
      expect(iterator.next()).deep.equal({ value: 5, done: false });
      expect(buffer[0]).eq(5);
      expect(iterator.next()).deep.equal({ value: undefined, done: true });
      expect(buffer[0]).eq(5);
      expect(iterator.next()).deep.equal({ value: undefined, done: true });
      expect(buffer[0]).eq(5);
    });

    it("iterates, buffer 2, bitmap 1", () => {
      const bitmap = new RoaringBitmap32([5]);
      const buffer = new Uint32Array(2);
      const iterator = new RoaringBitmap32ReverseIterator(bitmap, buffer);
      expect(Array.from(buffer)).deep.equal([0, 0]);
      expect(iterator.next()).deep.equal({ value: 5, done: false });
      expect(Array.from(buffer)).deep.equal([5, 0]);
      expect(iterator.next()).deep.equal({ value: undefined, done: true });
      expect(Array.from(buffer)).deep.equal([5, 0]);
      expect(iterator.next()).deep.equal({ value: undefined, done: true });
      expect(Array.from(buffer)).deep.equal([5, 0]);
    });

    it("iterates, buffer 1, bitmap 3", () => {
      const bitmap = new RoaringBitmap32([5, 7, 9]);
      const buffer = new Uint32Array(1);
      const iterator = new RoaringBitmap32ReverseIterator(bitmap, buffer);
      expect(buffer[0]).eq(0);
      expect(iterator.next()).deep.equal({ value: 9, done: false });
      expect(buffer[0]).eq(9);
      expect(iterator.next()).deep.equal({ value: 7, done: false });
      expect(buffer[0]).eq(7);
      expect(iterator.next()).deep.equal({ value: 5, done: false });
      expect(buffer[0]).eq(5);
    });

    it("iterates, buffer 2, bitmap 3", () => {
      const bitmap = new RoaringBitmap32([5, 7, 9]);
      const buffer = new Uint32Array(2);
      const iterator = new RoaringBitmap32ReverseIterator(bitmap, buffer);
      expect(Array.from(buffer)).deep.equal([0, 0]);
      expect(iterator.next()).deep.equal({ value: 9, done: false });
      expect(Array.from(buffer)).deep.equal([9, 7]);
      expect(iterator.next()).deep.equal({ value: 7, done: false });
      expect(Array.from(buffer)).deep.equal([9, 7]);
      expect(iterator.next()).deep.equal({ value: 5, done: false });
      expect(Array.from(buffer)).deep.equal([5, 7]);
      expect(buffer[0]).eq(5);
    });
  });

  it("throws if the bitmap is changed while iterating", () => {
    const bitmap = new RoaringBitmap32();
    bitmap.addRange(0, 1050);

    function doNothing(_v: any) {}

    let error: any;
    try {
      let n = 0;
      for (const v of new RoaringBitmap32ReverseIterator(bitmap, 256)) {
        if (n++ === 0) {
          bitmap.add(999999);
        } else {
          doNothing(v);
        }
      }
    } catch (e) {
      error = e;
    }
    expect(error.message).eq("RoaringBitmap32 iterator - bitmap changed while iterating");
  });

  describe("RoaringBitmap32 iterable", () => {
    it("returns a RoaringBitmap32ReverseIterator", () => {
      const bitmap = new RoaringBitmap32();
      const iterator = bitmap.reverseIterator();
      expect(iterator).to.be.instanceOf(RoaringBitmap32ReverseIterator);
      expect(typeof iterator.next).eq("function");
    });

    it("returns an empty iterator for an empty bitmap", () => {
      const bitmap = new RoaringBitmap32();
      const iterator = bitmap.reverseIterator();
      expect(iterator.next()).deep.equal({
        done: true,
        value: undefined,
      });
      expect(iterator.next()).deep.equal({
        done: true,
        value: undefined,
      });
    });
    it("iterates a non empty bitmap", () => {
      const bitmap = new RoaringBitmap32([0xffffffff, 3]);
      const iterator = bitmap.reverseIterator();
      expect(iterator.next()).deep.equal({
        done: false,
        value: 0xffffffff,
      });
      expect(iterator.next()).deep.equal({
        done: false,
        value: 3,
      });
      expect(iterator.next()).deep.equal({
        done: true,
        value: undefined,
      });
    });
  });
});
