import { describe, expect, it } from "vitest";
import RoaringBitmap32 from "../../RoaringBitmap32";

describe("RoaringBitmap32 operations", () => {
  describe("add, has", () => {
    it("works with multiple values", () => {
      const values = [100, 120, 130, 140, 150, 99, 1, 13, 64];
      const bitmap = new RoaringBitmap32();
      for (const value of values) {
        expect(bitmap.has(value)).eq(false);
        expect(bitmap.add(value)).eq(bitmap);
      }
      for (const value of values) {
        expect(bitmap.has(value)).eq(true);
      }
      expect(Array.from(bitmap)).deep.equal(values.slice().sort((a, b) => a - b));
    });

    it("accepts 32 bit big numbers", () => {
      const bitmap = new RoaringBitmap32();
      expect(bitmap.has(0x7fffffff)).eq(false);
      expect(bitmap.has(0xfffffffe)).eq(false);
      expect(bitmap.has(0xffffffff)).eq(false);
      bitmap.add(0x7fffffff);
      bitmap.add(0xfffffffe);
      bitmap.add(0xffffffff);
      expect(bitmap.has(0x7fffffff)).eq(true);
      expect(bitmap.has(0xfffffffe)).eq(true);
      expect(bitmap.has(0xffffffff)).eq(true);
      expect(Array.from(bitmap)).deep.equal([2147483647, 4294967294, 4294967295]);
    });

    it("returns false with some random ids on an empty bitmap", () => {
      const bitmap = new RoaringBitmap32();
      expect(bitmap.has(0)).eq(false);
      expect(bitmap.has(12340)).eq(false);
      expect(bitmap.has(0xffffffff)).eq(false);
      expect(bitmap.has(0xffffffff >>> 0)).eq(false);
    });

    it("returns false with invalid values on an empty bitmap", () => {
      const bitmap = new RoaringBitmap32();
      expect(bitmap.has(0.5)).eq(false);
      expect(bitmap.has(-12340)).eq(false);
      expect(bitmap.has("xxxx" as any)).eq(false);
      expect(bitmap.has(null as any)).eq(false);
      expect(bitmap.has(undefined as any)).eq(false);
      expect(bitmap.has(bitmap as any)).eq(false);
    });
  });

  describe("add, remove with multiple arguments", () => {
    it("can add multiple values at once", () => {
      const bitmap = new RoaringBitmap32();
      bitmap.add("1", 2, 3, 4, 5, 60, "199", 7, 8, 9, 10, 12.3, Number.NaN, Number.MAX_SAFE_INTEGER);
      expect(Array.from(bitmap)).deep.equal([1, 2, 3, 4, 5, 7, 8, 9, 10, 12, 60, 199]);
    });

    it("can add multiple values at once", () => {
      const bitmap = new RoaringBitmap32();
      expect(bitmap.tryAdd("1", 2, 3, 4, 5, 60, "199", 7, 8, 9, 10, 12.3, Number.NaN, Number.MAX_SAFE_INTEGER)).eq(
        true,
      );
      expect(bitmap.tryAdd("1", 2, 3, 4, 5, 60)).eq(false);
      expect(bitmap.tryAdd("1", 1000, 0xffffffff, 2)).eq(true);
      expect(Array.from(bitmap)).deep.equal([1, 2, 3, 4, 5, 7, 8, 9, 10, 12, 60, 199, 1000, 0xffffffff]);
    });

    it("can remove multiple values at once", () => {
      const bitmap = new RoaringBitmap32();
      bitmap.add("1", 2, 3, 4, 5, 60, "199", 7, 8, 9, 10, 12.3, Number.NaN, Number.MAX_SAFE_INTEGER);
      expect(Array.from(bitmap)).deep.equal([1, 2, 3, 4, 5, 7, 8, 9, 10, 12, 60, 199]);
      expect(bitmap.remove("1", 3, 4, 5, 60, "199", 7, 8, 9, 10, 12.3, Number.NaN, Number.MAX_SAFE_INTEGER)).eq(true);
      expect(Array.from(bitmap)).deep.equal([2]);
      expect(bitmap.remove("1", 2, 3, 4)).eq(true);
      expect(Array.from(bitmap)).deep.equal([]);
      expect(bitmap.remove("1", 2, 3, 4)).eq(false);
    });

    it("can delete multiple values at once", () => {
      const bitmap = new RoaringBitmap32();
      bitmap.add("1", 2, 3, 4, 5, 60, "199", 7, 8, 9, 10, 12.3, Number.NaN, Number.MAX_SAFE_INTEGER);
      expect(Array.from(bitmap)).deep.equal([1, 2, 3, 4, 5, 7, 8, 9, 10, 12, 60, 199]);
      expect(bitmap.delete("1", 3, 4, 5, 60, "199", 7, 8, 9, 10, 12.3, Number.NaN, Number.MAX_SAFE_INTEGER)).eq(true);
      expect(Array.from(bitmap)).deep.equal([2]);
      expect(bitmap.delete("1", 2, 3, 4)).eq(true);
      expect(Array.from(bitmap)).deep.equal([]);
      expect(bitmap.delete("1", 2, 3, 4)).eq(false);
    });
  });

  describe("addMany", () => {
    it("works with an array", () => {
      const values = [100, 120, 130, 140, 150, 99, 1, 13, 64];
      const sortedValues = values.slice().sort((a, b) => a - b);
      const bitmap = new RoaringBitmap32();
      bitmap.addMany(values);
      for (const value of values) {
        expect(bitmap.has(value)).eq(true);
      }

      let i = 0;
      for (const value of bitmap) {
        expect(value).eq(sortedValues[i++]);
      }

      expect(i).eq(sortedValues.length);
      expect(bitmap.size).eq(values.length);
      expect(Array.from(bitmap)).deep.equal(sortedValues);
    });

    it("works with an Uint32Array", () => {
      const values = [100, 120, 130, 140, 150, 99, 1, 13, 64];
      const bitmap = new RoaringBitmap32();
      bitmap.addMany(new Uint32Array(values));
      for (const value of values) {
        expect(bitmap.has(value)).eq(true);
      }
      const valuesSet = new Set(values);
      for (const value of bitmap) {
        expect(valuesSet.delete(value)).eq(true);
      }
      expect(valuesSet.size).eq(0);
      expect(Array.from(bitmap)).deep.equal(values.slice().sort((a, b) => a - b));
    });
  });

  describe("clear", () => {
    it("clear does nothing on an empty bitmap", () => {
      const bitmap = new RoaringBitmap32();
      expect(bitmap.clear()).eq(false);
      expect(bitmap.size).eq(0);
      expect(bitmap.isEmpty).eq(true);
    });

    it("removes all items", () => {
      const bitmap = new RoaringBitmap32([1, 2, 0x7fffffff, 3, 0xffffffff]);
      bitmap.clear();
      expect(bitmap.isEmpty).eq(true);
      expect(bitmap.size).eq(0);
      expect(bitmap.toArray()).deep.equal([]);
    });
  });

  describe("remove", () => {
    it("remove does nothing on an empty bitmap", () => {
      const bitmap = new RoaringBitmap32();
      expect(bitmap.remove(19)).to.be.false;
      expect(bitmap.size).eq(0);
    });

    it("remove one item", () => {
      const bitmap = new RoaringBitmap32([1, 5, 6]);
      expect(bitmap.remove(5)).to.be.true;
      expect(bitmap.toArray()).deep.equal([1, 6]);
      expect(bitmap.remove(5)).to.be.false;
      expect(bitmap.toArray()).deep.equal([1, 6]);
    });
  });

  describe("delete", () => {
    it("delete does nothing on an empty bitmap", () => {
      const bitmap = new RoaringBitmap32();
      expect(bitmap.delete(19)).eq(false);
      expect(bitmap.size).eq(0);
    });

    it("remove one item", () => {
      const bitmap = new RoaringBitmap32([1, 5, 6]);
      expect(bitmap.delete(5)).eq(true);
      expect(bitmap.toArray()).deep.equal([1, 6]);
      expect(bitmap.delete(5)).eq(false);
      expect(bitmap.toArray()).deep.equal([1, 6]);
    });
  });

  describe("removeMany", () => {
    it("removeMany does nothing on an empty bitmap", () => {
      const bitmap = new RoaringBitmap32();
      bitmap.removeMany([123]);
      expect(bitmap.size).eq(0);
    });

    it("removeMany removes one item", () => {
      const bitmap = new RoaringBitmap32([1, 2, 123, 3]);
      bitmap.removeMany([123]);
      expect(bitmap.toArray()).deep.equal([1, 2, 3]);
    });

    it("removeMany removes multiple items", () => {
      const bitmap = new RoaringBitmap32([1, 2, 123, 3, 4, 5, 6]);
      bitmap.removeMany([123, 5, 6]);
      expect(bitmap.toArray()).deep.equal([1, 2, 3, 4]);
      bitmap.removeMany([123, 5, 6]);
      expect(bitmap.toArray()).deep.equal([1, 2, 3, 4]);
    });

    it("removeMany removes multiple items (Uint32Array)", () => {
      const bitmap = new RoaringBitmap32([1, 2, 123, 3, 4, 5, 6]);
      bitmap.removeMany(new Uint32Array([123, 5, 6]));
      expect(bitmap.toArray()).deep.equal([1, 2, 3, 4]);
    });

    it("removeMany removes multiple items (RoaringBitmap32)", () => {
      const bitmap = new RoaringBitmap32([1, 2, 123, 3, 4, 5, 6]);
      bitmap.removeMany(new RoaringBitmap32([123, 5, 6]));
      expect(bitmap.toArray()).deep.equal([1, 2, 3, 4]);
    });

    it("removeMany removes multiple items (Uint32Array)", () => {
      const bitmap = new RoaringBitmap32([1, 2, 123, 3, 4, 5, 6]);
      bitmap.removeMany(new Uint32Array([123, 5, 6]));
      expect(bitmap.toArray()).deep.equal([1, 2, 3, 4]);
    });
  });

  describe("andInPlace", () => {
    it("does nothing with an empty bitmaps", () => {
      const a = new RoaringBitmap32();
      const b = new RoaringBitmap32();
      a.andInPlace(b);
      expect(a.isEmpty).eq(true);
    });

    it("does nothing with an empty bitmap and an empty array", () => {
      const a = new RoaringBitmap32();
      a.andInPlace([]);
      expect(a.isEmpty).eq(true);
    });

    it("does nothing with an empty bitmap and a non empty array", () => {
      const a = new RoaringBitmap32();
      a.andInPlace([1, 2, 3]);
      expect(a.isEmpty).eq(true);
    });

    it("ands two RoaringBitmap32", () => {
      const a = new RoaringBitmap32([1, 2, 3, 4]);
      a.andInPlace(new RoaringBitmap32([1, 4]));
      expect(a.toArray()).deep.equal([1, 4]);
    });

    it("ands one RoaringBitmap32 and one array", () => {
      const a = new RoaringBitmap32([1, 2, 3, 4]);
      a.andInPlace([1, 4]);
      expect(a.toArray()).deep.equal([1, 4]);
    });

    it("ands one RoaringBitmap32 and one UInt32Array", () => {
      const a = new RoaringBitmap32([1, 2, 3, 4]);
      a.andInPlace(new Uint32Array([1, 4]));
      expect(a.toArray()).deep.equal([1, 4]);
    });

    it("ands one RoaringBitmap32 and one Int32Array", () => {
      const a = new RoaringBitmap32([1, 2, 3, 4]);
      a.andInPlace(new Int32Array([1, 4]));
      expect(a.toArray()).deep.equal([1, 4]);
    });
  });

  describe("andNotInPlace", () => {
    it("does nothing with two empty bitmaps", () => {
      const a = new RoaringBitmap32();
      const b = new RoaringBitmap32();
      a.andNotInPlace(b);
      expect(a.isEmpty).eq(true);
    });

    it("does nothing with an empty bitmap and empty array", () => {
      const a = new RoaringBitmap32();
      a.andNotInPlace([]);
      expect(a.isEmpty).eq(true);
    });

    it("does nothing with an empty bitmap and a non empty array", () => {
      const a = new RoaringBitmap32();
      a.andNotInPlace([1, 2, 3]);
      expect(a.isEmpty).eq(true);
    });

    it("and not two bitmaps", () => {
      const a = new RoaringBitmap32([1, 2, 3, 4]);
      a.andNotInPlace(new RoaringBitmap32([1, 2]));
      expect(a.toArray()).deep.equal([3, 4]);
    });

    it("and not a bitmap and an array", () => {
      const a = new RoaringBitmap32([1, 2, 3, 4]);
      a.andNotInPlace([1, 2]);
      expect(a.toArray()).deep.equal([3, 4]);
    });

    it("and not a bitmap and an UInt32Array", () => {
      const a = new RoaringBitmap32([1, 2, 3, 4]);
      a.andNotInPlace(new Uint32Array([1, 2]));
      expect(a.toArray()).deep.equal([3, 4]);
    });

    it("and not a bitmap and an Int32Array", () => {
      const a = new RoaringBitmap32([1, 2, 3, 4]);
      a.andNotInPlace(new Int32Array([1, 2]));
      expect(a.toArray()).deep.equal([3, 4]);
    });
  });

  describe("orInPlace", () => {
    it("does nothing with two empty bitmaps", () => {
      const a = new RoaringBitmap32();
      const b = new RoaringBitmap32();
      a.orInPlace(b);
      expect(a.isEmpty).eq(true);
    });

    it("does nothing with an empty bitmap and one empty array", () => {
      const a = new RoaringBitmap32();
      a.orInPlace([]);
      expect(a.isEmpty).eq(true);
    });

    it("adds values to an empty bitmap (array)", () => {
      const a = new RoaringBitmap32();
      a.orInPlace([1, 2, 3]);
      expect(a.toArray()).deep.equal([1, 2, 3]);
    });

    it("adds values to an empty bitmap (Uint32Array)", () => {
      const a = new RoaringBitmap32();
      a.orInPlace(new Uint32Array([1, 2, 3]));
      expect(a.toArray()).deep.equal([1, 2, 3]);
    });

    it("adds values to an empty bitmap (Int32Array)", () => {
      const a = new RoaringBitmap32();
      a.orInPlace(new Int32Array([1, 2, 3]));
      expect(a.toArray()).deep.equal([1, 2, 3]);
    });

    it("adds values to an empty bitmap (RoaringBitmap32)", () => {
      const a = new RoaringBitmap32();
      a.orInPlace(new RoaringBitmap32([1, 2, 3]));
      expect(a.toArray()).deep.equal([1, 2, 3]);
    });

    it("ors two bitmaps", () => {
      const a = new RoaringBitmap32([1, 3, 5]);
      a.orInPlace(new RoaringBitmap32([2, 4, 5]));
      expect(a.toArray()).deep.equal([1, 2, 3, 4, 5]);
    });

    it("ors a bitmap and an array", () => {
      const a = new RoaringBitmap32([1, 3, 5]);
      a.orInPlace([2, 4, 5]);
      expect(a.toArray()).deep.equal([1, 2, 3, 4, 5]);
    });

    it("ors a bitmap and an Uint32Array", () => {
      const a = new RoaringBitmap32([1, 3, 5]);
      a.orInPlace(new Uint32Array([2, 4, 5]));
      expect(a.toArray()).deep.equal([1, 2, 3, 4, 5]);
    });

    it("ors a bitmap and an Int32Array", () => {
      const a = new RoaringBitmap32([1, 3, 5]);
      a.orInPlace(new Int32Array([2, 4, 5]));
      expect(a.toArray()).deep.equal([1, 2, 3, 4, 5]);
    });
  });

  describe("xorInPlace", () => {
    it("does nothing with two empty bitmaps", () => {
      const a = new RoaringBitmap32();
      const b = new RoaringBitmap32();
      a.xorInPlace(b);
      expect(a.isEmpty).eq(true);
    });

    it("does nothing with an empty bitmap and an empty array", () => {
      const a = new RoaringBitmap32();
      a.xorInPlace([]);
      expect(a.isEmpty).eq(true);
    });

    it("adds values to an empty bitmap (array)", () => {
      const a = new RoaringBitmap32();
      a.xorInPlace([1, 2, 3]);
      expect(a.toArray()).deep.equal([1, 2, 3]);
    });

    it("adds values to an empty bitmap (Uint32Array)", () => {
      const a = new RoaringBitmap32();
      a.xorInPlace(new Uint32Array([1, 2, 3]));
      expect(a.toArray()).deep.equal([1, 2, 3]);
    });

    it("adds values to an empty bitmap (Int32Array)", () => {
      const a = new RoaringBitmap32();
      a.xorInPlace(new Int32Array([1, 2, 3]));
      expect(a.toArray()).deep.equal([1, 2, 3]);
    });

    it("xors (array)", () => {
      const a = new RoaringBitmap32([1, 2, 3]);
      a.xorInPlace([1, 2, 3, 4]);
      expect(a.toArray()).deep.equal([4]);
    });

    it("xors (Uint32Array)", () => {
      const a = new RoaringBitmap32([1, 2, 3]);
      a.xorInPlace(new Uint32Array([1, 2, 3, 4]));
      expect(a.toArray()).deep.equal([4]);
    });

    it("xors (Int32Array)", () => {
      const a = new RoaringBitmap32([1, 2, 3]);
      a.xorInPlace(new Int32Array([1, 2, 3, 4]));
      expect(a.toArray()).deep.equal([4]);
    });

    it("xors (bitmap)", () => {
      const a = new RoaringBitmap32([1, 2, 4, 3]);
      a.xorInPlace(new RoaringBitmap32([1, 2, 3]));
      expect(a.toArray()).deep.equal([4]);
    });
  });

  describe("copyFrom", () => {
    describe("to an empty bitmap", () => {
      it("does nothing when copying (undefined)", () => {
        const bitmap = new RoaringBitmap32();
        bitmap.copyFrom(undefined);
        expect(bitmap.size).eq(0);
        expect(bitmap.isEmpty).eq(true);
      });

      it("does nothing when copying (null)", () => {
        const bitmap = new RoaringBitmap32();
        bitmap.copyFrom(null);
        expect(bitmap.size).eq(0);
        expect(bitmap.isEmpty).eq(true);
      });

      it("does nothing when copying self (empty)", () => {
        const bitmap = new RoaringBitmap32();
        bitmap.copyFrom(bitmap);
        expect(bitmap.size).eq(0);
        expect(bitmap.isEmpty).eq(true);
      });

      it("does nothing when copying an empty array", () => {
        const bitmap = new RoaringBitmap32();
        bitmap.copyFrom([]);
        expect(bitmap.size).eq(0);
        expect(bitmap.isEmpty).eq(true);
      });

      it("does nothing when copying an empty Uint32Array", () => {
        const bitmap = new RoaringBitmap32();
        bitmap.copyFrom(new Uint32Array([]));
        expect(bitmap.size).eq(0);
        expect(bitmap.isEmpty).eq(true);
      });

      it("does nothing when copying an empty Int32Array", () => {
        const bitmap = new RoaringBitmap32();
        bitmap.copyFrom(new Int32Array([]));
        expect(bitmap.size).eq(0);
        expect(bitmap.isEmpty).eq(true);
      });

      it("does nothing when copying an empty RoaringBitmap32", () => {
        const bitmap = new RoaringBitmap32();
        bitmap.copyFrom(new RoaringBitmap32());
        expect(bitmap.size).eq(0);
        expect(bitmap.isEmpty).eq(true);
      });
    });

    describe("to a bitmap with content", () => {
      it("clears if passed undefined", () => {
        const bitmap = new RoaringBitmap32([1, 2, 3]);
        bitmap.copyFrom(undefined);
        expect(bitmap.size).eq(0);
        expect(bitmap.isEmpty).eq(true);
      });

      it("clears if passed null", () => {
        const bitmap = new RoaringBitmap32([1, 2, 3]);
        bitmap.copyFrom(null);
        expect(bitmap.size).eq(0);
        expect(bitmap.isEmpty).eq(true);
      });

      it("clears if passed an empty array", () => {
        const bitmap = new RoaringBitmap32([1, 2, 3]);
        bitmap.copyFrom([]);
        expect(bitmap.size).eq(0);
        expect(bitmap.isEmpty).eq(true);
      });

      it("clears if passed an empty RoaringBitmap32", () => {
        const bitmap = new RoaringBitmap32([1, 2, 3]);
        bitmap.copyFrom(new RoaringBitmap32());
        expect(bitmap.size).eq(0);
        expect(bitmap.isEmpty).eq(true);
      });

      it("replace with the given values (array)", () => {
        const bitmap = new RoaringBitmap32([1, 2, 3]);
        bitmap.copyFrom([4, 6, 8, 10]);
        expect(bitmap.toArray()).deep.equal([4, 6, 8, 10]);
      });

      it("replace with the given values (Uint32Array)", () => {
        const bitmap = new RoaringBitmap32([1, 2, 3]);
        bitmap.copyFrom(new Uint32Array([4, 6, 8, 10]));
        expect(bitmap.toArray()).deep.equal([4, 6, 8, 10]);
      });

      it("replace with the given values (Int32Array)", () => {
        const bitmap = new RoaringBitmap32([1, 2, 3]);
        bitmap.copyFrom(new Int32Array([4, 6, 8, 10]));
        expect(bitmap.toArray()).deep.equal([4, 6, 8, 10]);
      });

      it("replace with the given values (bitmap)", () => {
        const bitmap = new RoaringBitmap32([1, 2, 3]);
        bitmap.copyFrom(new RoaringBitmap32([4, 6, 8, 10]));
        expect(bitmap.toArray()).deep.equal([4, 6, 8, 10]);
      });
    });
  });

  describe("at", () => {
    it("returns the value at the given index", () => {
      const bitmap = new RoaringBitmap32([1, 12, 30, 0xffff]);
      expect(bitmap.at(0)).eq(1);
      expect(bitmap.at(1)).eq(12);
      expect(bitmap.at(2)).eq(30);
      expect(bitmap.at(3)).eq(0xffff);
      expect(bitmap.at(4)).eq(undefined);
      expect(bitmap.at(5)).eq(undefined);
      expect(bitmap.at(-5)).eq(undefined);

      expect(bitmap.at(1.5)).eq(12);
      expect(bitmap.at("1.5" as any)).eq(12);
    });

    it("works with negative indices", () => {
      const bitmap = new RoaringBitmap32([1, 12, 3]);
      expect(bitmap.at(-1)).eq(12);
      expect(bitmap.at(-1.4)).eq(12);
      expect(bitmap.at(-2)).eq(3);
      expect(bitmap.at(-2.9)).eq(3);
      expect(bitmap.at(-3)).eq(1);
      expect(bitmap.at("-3" as any)).eq(1);
      expect(bitmap.at(-4)).eq(undefined);
    });
  });
});
