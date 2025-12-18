import { describe, expect, it } from "vitest";
import RoaringBitmap32 from "../../RoaringBitmap32";

describe("RoaringBitmap32 basic", () => {
  describe("minimum", () => {
    it("returns 4294967295 for an empty bitmap", () => {
      expect(new RoaringBitmap32().minimum()).eq(4294967295);
    });
    it("returns the minimum value", () => {
      const bitmap = new RoaringBitmap32([5, 3, 1, 2, 9]);
      expect(bitmap.minimum()).eq(1);
    });
  });

  describe("maximum", () => {
    it("returns 0 for an empty bitmap", () => {
      expect(new RoaringBitmap32().maximum()).eq(0);
    });
    it("returns the maximum value", () => {
      const bitmap = new RoaringBitmap32([5, 3, 1, 2, 9]);
      expect(bitmap.maximum()).eq(9);
    });
  });

  describe("toString", () => {
    it('returns "RoaringBitmap32', () => {
      expect(new RoaringBitmap32().toString()).eq("RoaringBitmap32");
    });
  });

  describe("contentToString", () => {
    it('returns "[]" for an empty bitmap', () => {
      expect(new RoaringBitmap32().contentToString()).eq("[]");
    });
    it("generates a valid string for 1 value", () => {
      const bitmap = new RoaringBitmap32([1]);
      expect(bitmap.contentToString()).eq("[1]");
    });
    it("generates a valid string for few values", () => {
      const values = [100, 200, 201, 202, 203, 204, 300, 0x7fffffff, 0xffffffff];
      const bitmap = new RoaringBitmap32(values);
      expect(bitmap.contentToString()).eq("[100,200,201,202,203,204,300,2147483647,4294967295]");
    });
    it("supports maxLength correctly", () => {
      const values = [100, 200, 201, 202, 203, 204, 300, 0x7fffffff, 0xffffffff];
      const bitmap = new RoaringBitmap32(values);
      expect(bitmap.contentToString(13)).eq("[100,200,201,202...]");
    });
  });

  describe("join", () => {
    it('returns "" for an empty bitmap', () => {
      expect(new RoaringBitmap32().join()).eq("");
    });
    it("generates a valid string for 1 value", () => {
      const bitmap = new RoaringBitmap32([1]);
      expect(bitmap.join()).eq("1");
    });
    it("generates a valid string for few values", () => {
      const values = [100, 200, 201, 202, 203, 204, 300, 0x7fffffff, 0xffffffff];
      const bitmap = new RoaringBitmap32(values);
      expect(bitmap.join()).eq("100,200,201,202,203,204,300,2147483647,4294967295");
    });
    it("supports custom separator", () => {
      const values = [100, 200, 201, 202, 203, 204, 300, 0x7fffffff, 0xffffffff];
      const bitmap = new RoaringBitmap32(values);
      expect(bitmap.join("xx")).eq("100xx200xx201xx202xx203xx204xx300xx2147483647xx4294967295");
    });
  });

  describe("clone", () => {
    it("returns a cloned empty bitmap", () => {
      const bitmap1 = new RoaringBitmap32();
      const bitmap2 = bitmap1.clone();
      expect(bitmap1 !== bitmap2).eq(true);
      expect(bitmap2).to.be.instanceOf(RoaringBitmap32);
      expect(bitmap2.size).eq(0);
      expect(bitmap2.isEmpty).eq(true);
    });

    it("returns a cloned bitmap", () => {
      const values = [1, 2, 100, 101, 200, 400, 0x7fffffff, 0xffffffff];
      const bitmap1 = new RoaringBitmap32(values);
      const bitmap2 = bitmap1.clone();
      expect(bitmap1 !== bitmap2).eq(true);
      expect(bitmap2).to.be.instanceOf(RoaringBitmap32);
      expect(bitmap2.size).eq(values.length);
      expect(bitmap2.isEmpty).eq(false);
      expect(Array.from(bitmap2.toUint32Array())).deep.equal(values);
    });
  });

  describe("select", () => {
    it("returns undefined for invalid values on an empty bitmap", () => {
      const bitmap = new RoaringBitmap32();
      expect(bitmap.select(null as any)).to.be.undefined;
      expect(bitmap.select(undefined as any)).to.be.undefined;
      expect(bitmap.select(-123 as any)).to.be.undefined;
      expect(bitmap.select([123] as any)).to.be.undefined;
      expect(bitmap.select(0)).to.be.undefined;
      expect(bitmap.select(100)).to.be.undefined;
      expect(bitmap.select(0x7fffffff)).to.be.undefined;
      expect(bitmap.select(3)).to.be.undefined;
    });

    it("returns the correct value", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5, 10, 100, 1000, 2000, 3000]);
      expect(bitmap.select(0)).eq(1);
      expect(bitmap.select(1)).eq(2);
      expect(bitmap.select(2)).eq(3);
      expect(bitmap.select(3)).eq(4);
      expect(bitmap.select(4)).eq(5);
      expect(bitmap.select(5)).eq(10);
      expect(bitmap.select(6)).eq(100);
      expect(bitmap.select(7)).eq(1000);
      expect(bitmap.select(8)).eq(2000);
      expect(bitmap.select(9)).eq(3000);
      expect(bitmap.select(10)).to.be.undefined;
    });
  });

  describe("rank", () => {
    it("returns 0 for invalid values", () => {
      const bitmap = new RoaringBitmap32();
      expect(bitmap.rank(null as any)).eq(0);
      expect(bitmap.rank(undefined as any)).eq(0);
      expect(bitmap.rank(-123 as any)).eq(0);
      expect(bitmap.rank([123] as any)).eq(0);
    });

    it("returns 0 with any value on an empty bitmap", () => {
      const bitmap = new RoaringBitmap32();
      expect(bitmap.rank(0)).eq(0);
      expect(bitmap.rank(100)).eq(0);
      expect(bitmap.rank(0x7fffffff)).eq(0);
      expect(bitmap.rank(0xffffffff)).eq(0);
    });

    it("returns the correct value", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5, 10, 100, 1000, 2000, 3000]);
      expect(bitmap.rank(0)).eq(0);
      expect(bitmap.rank(1)).eq(1);
      expect(bitmap.rank(2)).eq(2);
      expect(bitmap.rank(3)).eq(3);
      expect(bitmap.rank(4)).eq(4);
      expect(bitmap.rank(5)).eq(5);
      expect(bitmap.rank(10)).eq(6);
      expect(bitmap.rank(100)).eq(7);
      expect(bitmap.rank(1000)).eq(8);
      expect(bitmap.rank(2000)).eq(9);
      expect(bitmap.rank(3000)).eq(10);
    });
  });

  describe("toUint32Array", () => {
    it("returns an empty Uint32Array for an empty bitmap", () => {
      const a = new RoaringBitmap32().toUint32Array();
      expect(a).to.be.instanceOf(Uint32Array);
      expect(a).to.have.lengthOf(0);
    });

    it("returns an array with 1 element for 1 element", () => {
      const bitmap = new RoaringBitmap32([1]);
      const x = bitmap.toUint32Array();
      expect(x).to.be.instanceOf(Uint32Array);
      expect(x).to.have.lengthOf(1);
      expect(Array.from(x)).deep.equal([1]);
    });

    it("returns an array with multiple elements", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 0x7fffffff, 0xffffffff]);
      const x = bitmap.toUint32Array(undefined as any);
      expect(x).to.be.instanceOf(Uint32Array);
      expect(x).to.have.lengthOf(6);
      expect(Array.from(x)).deep.equal([1, 2, 10, 30, 0x7fffffff, 0xffffffff]);
    });

    it("throws if the output argument is invalid", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 0x7fffffff, 0xffffffff]);
      expect(() => bitmap.toUint32Array(null as any)).to.throw();
      expect(() => bitmap.toUint32Array({} as any)).to.throw();
      expect(() => bitmap.toUint32Array([] as any)).to.throw();
      expect(() => bitmap.toUint32Array(new Int8Array(100) as any)).to.throw();
    });

    it("writes the bitmap to the output array if the output array is the same size", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 0x7fffffff, 0xffffffff]);
      const output = new Uint32Array(6);
      expect(bitmap.toUint32Array(output)).to.eq(output);
      expect(Array.from(output)).deep.equal([1, 2, 10, 30, 0x7fffffff, 0xffffffff]);
    });

    it("supports SharedArrayBuffer", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 0x7fffffff, 0xffffffff]);
      const output = new Uint32Array(new SharedArrayBuffer(6 * 4));
      expect(bitmap.toUint32Array(output)).to.eq(output);
      expect(Array.from(output)).deep.equal([1, 2, 10, 30, 0x7fffffff, 0xffffffff]);
    });

    it("writes the bitmap to the output array if the output array is bigger", () => {
      const data = [1, 2, 10, 30, 0x7fffffff, 0xffffffff];
      const bitmap = new RoaringBitmap32(data);
      const output = new Uint32Array(20);
      const result = bitmap.toUint32Array(output);
      expect(output.buffer).to.eq(result.buffer);
      expect(Array.from(result)).to.deep.eq(data);
    });

    it("writes the bitmap to the output array if the output array is smaller", () => {
      const data = [1, 2, 10, 30, 40, 50, 55, 0x7fffffff, 0xffffffff];
      const bitmap = new RoaringBitmap32(data);
      const output = new Uint32Array(5);
      const result = bitmap.toUint32Array(output);
      expect(output.buffer).to.eq(result.buffer);
      expect(Array.from(result)).to.deep.eq(data.slice(0, 5));
    });

    it("with maxSize, returns an array with the specified number of elements", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 40, 50, 55, 0x7fffffff, 0xffffffff]);
      const x = bitmap.toUint32Array(5);
      expect(x).to.be.instanceOf(Uint32Array);
      expect(x).to.have.lengthOf(5);
      expect(Array.from(x)).deep.equal([1, 2, 10, 30, 40]);
      expect(bitmap.toUint32Array(0)).to.have.lengthOf(0);
      expect(bitmap.toUint32Array(-10)).to.have.lengthOf(0);
    });

    it("with maxSize greater than the bitmap cardinality, returns all elements", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 40, 50, 55, 0x7fffffff, 0xffffffff]);
      expect(bitmap.toUint32Array(10)).to.have.lengthOf(9);
      expect(bitmap.toUint32Array(125)).to.have.lengthOf(9);
      expect(bitmap.toUint32Array(Number.MAX_SAFE_INTEGER)).to.have.lengthOf(9);
      expect(bitmap.toUint32Array(Number.POSITIVE_INFINITY)).to.have.lengthOf(9);
    });
  });

  describe("toUint32ArrayAsync", () => {
    it("returns an empty Uint32Array for an empty bitmap", async () => {
      const a = await new RoaringBitmap32().toUint32ArrayAsync();
      expect(a).to.be.instanceOf(Uint32Array);
      expect(a).to.have.lengthOf(0);
    });

    it("returns an array with 1 element for 1 element", async () => {
      const bitmap = new RoaringBitmap32([1]);
      const x = await bitmap.toUint32ArrayAsync();
      expect(x).to.be.instanceOf(Uint32Array);
      expect(x).to.have.lengthOf(1);
      expect(Array.from(x)).deep.equal([1]);
    });

    it("returns an array with multiple elements", async () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 0x7fffffff, 0xffffffff]);
      const x = await bitmap.toUint32ArrayAsync(undefined as any);
      expect(x).to.be.instanceOf(Uint32Array);
      expect(x).to.have.lengthOf(6);
      expect(Array.from(x)).deep.equal([1, 2, 10, 30, 0x7fffffff, 0xffffffff]);
    });

    it("throws if the output argument is invalid", async () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 0x7fffffff, 0xffffffff]);
      await expect(bitmap.toUint32ArrayAsync(null as any)).rejects.toThrow(Error);
      await expect(bitmap.toUint32ArrayAsync({} as any)).rejects.toThrow(Error);
      await expect(bitmap.toUint32ArrayAsync([] as any)).rejects.toThrow(Error);
      await expect(bitmap.toUint32ArrayAsync(new Int8Array(1) as any)).rejects.toThrow(Error);
    });

    it("writes the bitmap to the output array", async () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 0x7fffffff, 0xffffffff]);
      const output = new Uint32Array(6);
      expect(await bitmap.toUint32ArrayAsync(output)).to.eq(output);
      expect(Array.from(output)).deep.equal([1, 2, 10, 30, 0x7fffffff, 0xffffffff]);
    });

    it("writes the bitmap to the output SharedArrayBuffer", async () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 0x7fffffff, 0xffffffff]);
      const output = new SharedArrayBuffer(6 * 4);
      const result = await bitmap.toUint32ArrayAsync(output);
      expect(result.buffer).to.eq(output);
      expect(Array.from(result)).deep.equal([1, 2, 10, 30, 0x7fffffff, 0xffffffff]);
    });

    it("writes the bitmap to the output array if the output array is the same size", async () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 0x7fffffff, 0xffffffff]);
      const output = new Uint32Array(6);
      expect(await bitmap.toUint32ArrayAsync(output)).to.eq(output);
      expect(Array.from(output)).deep.equal([1, 2, 10, 30, 0x7fffffff, 0xffffffff]);
    });

    it("writes the bitmap to the output array if the output array is bigger", async () => {
      const data = [1, 2, 10, 30, 0x7fffffff, 0xffffffff];
      const bitmap = new RoaringBitmap32(data);
      const output = new Uint32Array(20);
      const result = await bitmap.toUint32ArrayAsync(output);
      expect(output.buffer).to.eq(result.buffer);
      expect(Array.from(result)).to.deep.eq(data);
    });

    it("writes the bitmap to the output array if the output array is smaller", async () => {
      const data = [1, 2, 10, 30, 40, 50, 55, 0x7fffffff, 0xffffffff];
      const bitmap = new RoaringBitmap32(data);
      const output = new Uint32Array(5);
      const result = await bitmap.toUint32ArrayAsync(output);
      expect(output.buffer).to.eq(result.buffer);
      expect(Array.from(result)).to.deep.eq(data.slice(0, 5));
    });

    it("freezes the bitmap while in progress", async () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 0x7fffffff, 0xffffffff]);
      const p = bitmap.toUint32ArrayAsync();
      expect(bitmap.isFrozen).to.be.true;
      await p;
      expect(bitmap.isFrozen).to.be.false;
    });
  });

  describe("rangeUint32Array", () => {
    it("returns an empty Uint32Array for an empty bitmap", () => {
      const a = new RoaringBitmap32().rangeUint32Array(0, 10);
      expect(a).to.be.instanceOf(Uint32Array);
      expect(a).to.have.lengthOf(0);
    });

    it("returns a paginated array with multiple elements", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 50, 70, 100]);
      const x = bitmap.rangeUint32Array(0, 3);
      expect(x).to.be.instanceOf(Uint32Array);
      expect(x).to.have.lengthOf(3);
      expect(Array.from(x)).deep.equal([1, 2, 10]);
    });

    it("returns a paginated array with offset", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 50, 70, 100]);
      const x = bitmap.rangeUint32Array(2, 3);
      expect(x).to.be.instanceOf(Uint32Array);
      expect(x).to.have.lengthOf(3);
      expect(Array.from(x)).deep.equal([10, 30, 50]);
    });

    it("returns a paginated array with offset almost out of scope", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 50, 70, 100]);
      const x = bitmap.rangeUint32Array(6, 3);
      expect(x).to.be.instanceOf(Uint32Array);
      expect(x).to.have.lengthOf(1);
      expect(Array.from(x)).deep.equal([100]);
    });

    it("returns a paginated array with offset out of scope", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 50, 70, 100]);
      const x = bitmap.rangeUint32Array(10, 3);
      expect(x).to.be.instanceOf(Uint32Array);
      expect(x).to.have.lengthOf(0);
      expect(Array.from(x)).deep.equal([]);
    });

    it("returns a paginated array with limit out of scope", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 50, 70, 100]);
      const x = bitmap.rangeUint32Array(0, 100);
      expect(x).to.be.instanceOf(Uint32Array);
      expect(x).to.have.lengthOf(7);
    });

    it("returns a paginated array with limit and offset out of scope", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 50, 70, 100]);
      const x = bitmap.rangeUint32Array(5, 100);
      expect(x).to.be.instanceOf(Uint32Array);
      expect(x).to.have.lengthOf(2);
    });

    it("throws if the output argument is invalid", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 50, 70, 100]);
      expect(() => bitmap.rangeUint32Array(5, 7, null as any)).to.throw();
      expect(() => bitmap.rangeUint32Array(5, 7, 123 as any)).to.throw();
      expect(() => bitmap.rangeUint32Array(5, 7, {} as any)).to.throw();
      expect(() => bitmap.rangeUint32Array(5, 7, [] as any)).to.throw();
      expect(() => bitmap.rangeUint32Array(5, 7, new Int8Array(1) as any)).to.throw();
    });

    it("writes the bitmap to the output array", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 50, 70, 100]);
      const output = new Uint32Array(3);
      expect(bitmap.rangeUint32Array(0, 3, output)).to.eq(output);
      expect(Array.from(output)).deep.equal([1, 2, 10]);
    });

    // All 3 overloads
    it("returns a paginated array with multiple elements", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 50, 70, 100]);
      const x = bitmap.rangeUint32Array(0, 3);
      expect(x).to.be.instanceOf(Uint32Array);
      expect(x).to.have.lengthOf(3);
      expect(Array.from(x)).deep.equal([1, 2, 10]);
    });

    it("returns a paginated array with offset", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 50, 70, 100]);
      const x = bitmap.rangeUint32Array(2, 3);
      expect(x).to.be.instanceOf(Uint32Array);
      expect(x).to.have.lengthOf(3);
      expect(Array.from(x)).deep.equal([10, 30, 50]);
    });

    it("returns a paginated array with offset almost out of scope", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 50, 70, 100]);
      const x = bitmap.rangeUint32Array(6, 3);
      expect(x).to.be.instanceOf(Uint32Array);
      expect(x).to.have.lengthOf(1);
      expect(Array.from(x)).deep.equal([100]);
    });

    it("returns a paginated array with offset out of scope", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 50, 70, 100]);
      const x = bitmap.rangeUint32Array(10, 3);
      expect(x).to.be.instanceOf(Uint32Array);
      expect(x).to.have.lengthOf(0);
      expect(Array.from(x)).deep.equal([]);
    });

    it("returns a paginated array with limit out of scope", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 50, 70, 100]);
      const x = bitmap.rangeUint32Array(0, 100);
      expect(x).to.be.instanceOf(Uint32Array);
      expect(x).to.have.lengthOf(7);
      expect(Array.from(x)).deep.equal([1, 2, 10, 30, 50, 70, 100]);
    });

    it("returns a paginated array with limit and offset out of scope", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 50, 70, 100]);
      const x = bitmap.rangeUint32Array(5, 100);
      expect(x).to.be.instanceOf(Uint32Array);
      expect(x).to.have.lengthOf(2);
      expect(Array.from(x)).deep.equal([70, 100]);
    });

    it("limits correctly if the output array is too small", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 50, 70, 100]);
      const output = new Uint32Array(2);
      expect(bitmap.rangeUint32Array(0, 3, output)).to.eq(output);
      expect(Array.from(output)).deep.equal([1, 2]);
    });

    it("limits correctly if the output array is too small", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 50, 70, 100]);
      const output = new Uint32Array(3);
      const result = bitmap.rangeUint32Array(0, 2, output);
      expect(Array.from(result)).to.deep.eq([1, 2]);
      expect(Array.from(output)).deep.equal([1, 2, 0]);
      expect(result.buffer).eq(output.buffer);
    });

    it("limits correctly if the output array is too small with two arguments", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 50, 70, 100]);
      const output = new Uint32Array(3);
      const result = bitmap.rangeUint32Array(4, output);
      expect(Array.from(result)).to.deep.eq([50, 70, 100]);
      expect(result).eq(output);
    });

    it("limits correctly if the output array is too small with two arguments", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 50, 70, 100]);
      const output = new Uint32Array(3);
      const result = bitmap.rangeUint32Array(output, 4);
      expect(Array.from(result)).to.deep.eq([50, 70, 100]);
      expect(result).eq(output);
    });

    it("limits correctly if the output array is too small with 3 arguments", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 50, 70, 100]);
      const output = new Uint32Array(3);
      const result = bitmap.rangeUint32Array(output, 4, 200);
      expect(Array.from(result)).to.deep.eq([50, 70, 100]);
      expect(result).eq(output);
    });

    it("accepts a single argument of UInt32Array", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 50, 70, 100]);
      const output = new Uint32Array(3);
      const result = bitmap.rangeUint32Array(output);
      expect(Array.from(result)).to.deep.eq([1, 2, 10]);
      expect(result).eq(output);
    });

    it("accepts a single argument of SharedArrayBuffer", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 50, 70, 100]);
      const output = new SharedArrayBuffer(3 * 4);
      const result = bitmap.rangeUint32Array(output);
      expect(Array.from(result)).to.deep.eq([1, 2, 10]);
      expect(result.buffer).eq(output);
    });
  });

  describe("toArray", () => {
    it("returns an empty array for an empty bitmap", () => {
      expect(new RoaringBitmap32().toArray()).deep.equal([]);
    });

    it("returns an array with 1 element for 1 element", () => {
      const bitmap = new RoaringBitmap32([1]);
      expect(bitmap.toArray()).deep.equal([1]);
    });

    it("returns an array with multiple elements", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 0x7fffffff, 0xffffffff]);
      expect(bitmap.toArray()).deep.equal([1, 2, 10, 30, 0x7fffffff, 0xffffffff]);
    });

    it("limits using maxLength argument", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 0x7fffffff, 0xffffffff]);
      expect(bitmap.toArray(3)).deep.equal([1, 2, 10]);
    });

    it("throws if the argument is not a valid array", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 0x7fffffff, 0xffffffff]);
      expect(() => bitmap.toArray(null as any)).to.throw();
      expect(() => bitmap.toArray({} as any)).to.throw();
      expect(() => bitmap.toArray(new Int8Array(1) as any)).to.throw();
    });

    it("appends to an empty output array", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 0x7fffffff, 0xffffffff]);
      const output: number[] = [];
      expect(bitmap.toArray(output)).to.eq(output);
      expect(Array.from(output)).deep.equal([1, 2, 10, 30, 0x7fffffff, 0xffffffff]);
    });

    it("appends to a non empty output array", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30]);
      const output = [100];
      expect(bitmap.toArray(output)).to.eq(output);
      expect(Array.from(output)).deep.equal([100, 1, 2, 10, 30]);
    });

    it("overwrite values in a non empty output array, with an offset and maximum length", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30]);
      const output = [100, 200, 300, 400, 500, 600];
      expect(bitmap.toArray(output, 2, 2)).to.eq(output);
      expect(Array.from(output)).deep.equal([100, 200, 1, 2, 500, 600]);
    });
  });

  describe("toSet", () => {
    it("returns an empty set for an empty bitmap", () => {
      const set = new RoaringBitmap32().toSet();
      expect(set).to.be.instanceOf(Set);
      expect(Array.from(set)).deep.equal([]);
    });

    it("returns an array with 1 element for 1 element", () => {
      const set = new RoaringBitmap32([1]).toSet();
      expect(set).to.be.instanceOf(Set);
      expect(Array.from(set)).deep.equal([1]);
    });

    it("returns an array with multiple elements", () => {
      const values = [1, 2, 10, 30, 0x7fffffff, 0xffffffff];
      const set = new RoaringBitmap32(values).toSet();
      expect(set).to.be.instanceOf(Set);
      expect(Array.from(set)).deep.equal(values);
    });

    it("limits with maxLength", () => {
      const values = [1, 2, 10, 30, 0x7fffffff, 0xffffffff];
      const set = new RoaringBitmap32(values).toSet(3);
      expect(set).to.be.instanceOf(Set);
      expect(Array.from(set)).deep.equal([1, 2, 10]);
    });

    it("throws if the given set is invalid", () => {
      const bitmap = new RoaringBitmap32([1, 2, 10, 30, 0x7fffffff, 0xffffffff]);
      expect(() => bitmap.toSet(null as any)).to.throw();
      expect(() => bitmap.toSet({} as any)).to.throw();
      expect(() => bitmap.toSet([] as any)).to.throw();
    });

    it("fills the given set", () => {
      const values = [1, 2, 10, 30, 0x7fffffff, 0xffffffff];
      const set = new Set([77, 1]);
      const bitmap = new RoaringBitmap32(values);
      expect(bitmap.toSet(set)).to.eq(set);
      expect(Array.from(set)).deep.equal([77, ...values]);
    });

    it("fills the given set, limiting to maxLength", () => {
      const values = [1, 2, 10, 30, 0x7fffffff, 0xffffffff];
      const set = new Set([77, 1]);
      const bitmap = new RoaringBitmap32(values);
      expect(bitmap.toSet(set, 3)).to.eq(set);
      expect(Array.from(set)).deep.equal([77, 1, 2, 10]);
    });
  });

  describe("toJSON", () => {
    it("returns an empty array with an empty bitmap", () => {
      expect(new RoaringBitmap32().toJSON()).deep.equal([]);
    });

    it("works with JSON.stringify with an empty bitmap", () => {
      const bitmap = new RoaringBitmap32();
      expect(JSON.stringify(bitmap)).eq("[]");
    });

    it("returns an array with all the values", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3]);
      expect(bitmap.toJSON()).deep.equal([1, 2, 3]);
    });

    it("works with JSON.stringify", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3]);
      expect(JSON.stringify(bitmap)).eq("[1,2,3]");
    });
  });

  describe("statistics", () => {
    it("returns a statistics object for an empty bitmap", () => {
      const bitmap = new RoaringBitmap32();
      const statistics = bitmap.statistics();
      expect(statistics).deep.equal({
        containers: 0,
        arrayContainers: 0,
        runContainers: 0,
        bitsetContainers: 0,
        valuesInArrayContainers: 0,
        valuesInRunContainers: 0,
        valuesInBitsetContainers: 0,
        bytesInArrayContainers: 0,
        bytesInRunContainers: 0,
        bytesInBitsetContainers: 0,
        maxValue: 0,
        minValue: 4294967295,
        size: 0,
        isFrozen: false,
      });
    });

    it("returns valid statistics", () => {
      const rb = new RoaringBitmap32();
      expect(rb.addMany([1, 2, 3, 4, 6, 7])).eq(rb);
      expect(rb.addMany([999991, 999992, 999993, 999994, 999996, 999997])).eq(rb);
      expect(rb.statistics()).deep.equal({
        containers: 2,
        arrayContainers: 2,
        runContainers: 0,
        bitsetContainers: 0,
        valuesInArrayContainers: 12,
        valuesInRunContainers: 0,
        valuesInBitsetContainers: 0,
        bytesInArrayContainers: 24,
        bytesInRunContainers: 0,
        bytesInBitsetContainers: 0,
        maxValue: 999997,
        minValue: 1,
        size: 12,
        isFrozen: false,
      });
      rb.runOptimize();
      rb.shrinkToFit();
      const expected = {
        containers: 2,
        arrayContainers: 0,
        runContainers: 2,
        bitsetContainers: 0,
        valuesInArrayContainers: 0,
        valuesInRunContainers: 12,
        valuesInBitsetContainers: 0,
        bytesInArrayContainers: 0,
        bytesInRunContainers: 20,
        bytesInBitsetContainers: 0,
        maxValue: 999997,
        minValue: 1,
        size: 12,
        isFrozen: false,
      };
      expect(rb.statistics()).deep.equal(expected);
      rb.freeze();
      expect(rb.statistics()).deep.equal({ ...expected, isFrozen: true });
    });
  });

  describe("removeRunCompression", () => {
    it("does nothing with an empty bitmap", () => {
      const bitmap = new RoaringBitmap32();
      expect(bitmap.removeRunCompression()).eq(false);
    });
  });

  describe("runOptimize", () => {
    it("does nothing with an empty bitmap", () => {
      const bitmap = new RoaringBitmap32();
      expect(bitmap.runOptimize()).eq(false);
    });
    it("run optimizes", () => {
      const bitmap = new RoaringBitmap32();
      for (let i = 200; i > 0; --i) {
        bitmap.add(i);
      }
      expect(bitmap.runOptimize()).eq(true);
      expect(bitmap.statistics().runContainers).eq(1);
      bitmap.removeRunCompression();
      bitmap.shrinkToFit();
      expect(bitmap.statistics().runContainers).eq(0);
      expect(bitmap.size).eq(200);
    });
  });

  describe("shrinkToFit", () => {
    it("shrinks some memory", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3]);
      bitmap.removeMany([1, 2, 3]);
      expect(bitmap.shrinkToFit()).to.be.gt(0);
    });
  });

  describe("general tests", () => {
    it("allows adding 900 values", () => {
      const bitmap = new RoaringBitmap32();
      for (let i = 100; i < 1000; ++i) {
        bitmap.add(i);
      }
      expect(bitmap.size).eq(900);
      expect(bitmap.runOptimize()).eq(true);
      expect(bitmap.size).eq(900);
      expect(bitmap.minimum()).eq(100);
      expect(bitmap.maximum()).eq(999);
    });

    it('works with some "fancy" operations', () => {
      const rb1 = new RoaringBitmap32();
      rb1.add(1);
      rb1.add(2);
      rb1.add(3);
      rb1.add(4);
      rb1.add(5);
      rb1.add(100);
      rb1.add(1000);
      rb1.runOptimize();

      const rb2 = new RoaringBitmap32([3, 4, 1000]);
      rb2.runOptimize();
      const rb3 = new RoaringBitmap32();

      expect(rb1.size).eq(7);
      expect(rb1.has(1)).eq(true);
      expect(rb1.has(3)).eq(true);

      expect(rb1.has("1")).eq(false);
      expect(rb1.has("3")).eq(false);
      expect(rb1.has(0.2)).eq(false);
      expect(rb1.has(0.8)).eq(false);
      expect(rb1.has(0.9)).eq(false);
      expect(rb1.has(2.8)).eq(false);
      expect(rb1.has(3.1)).eq(false);

      expect(rb1.contentToString()).eq("[1,2,3,4,5,100,1000]");

      rb1.andInPlace(rb2);

      expect(rb1.includes(1)).eq(false);
      expect(rb1.includes(3)).eq(true);
      expect(rb1.has(1)).eq(false);
      expect(rb1.has(3)).eq(true);

      expect(rb1.has(undefined)).eq(false);
      expect(rb1.has(null)).eq(false);
      expect(rb1.has("")).eq(false);
      expect(rb1.has("1")).eq(false);
      expect(rb1.has("3")).eq(false);
      expect(rb1.has(1.1)).eq(false);

      expect(rb1.includes(undefined)).eq(false);
      expect(rb1.includes(null)).eq(false);
      expect(rb1.includes("")).eq(false);
      expect(rb1.includes("1")).eq(false);
      expect(rb1.includes("3")).eq(false);
      expect(rb1.includes(1.1)).eq(false);

      expect(rb1.size).eq(3);
      expect(rb1.contentToString()).eq("[3,4,1000]");

      rb3.add(5);
      rb3.orInPlace(rb1);

      expect(Array.from(rb3)).deep.equal([3, 4, 5, 1000]);
      expect(rb3.toArray()).deep.equal([3, 4, 5, 1000]);
      expect(Array.from(rb3.toUint32Array())).deep.equal([3, 4, 5, 1000]);

      const rb4 = RoaringBitmap32.orMany(rb1, rb2, rb3);
      expect(rb4.toArray()).deep.equal([3, 4, 5, 1000]);
    });
  });

  describe("indexOf", () => {
    it("returns -1 if the value is not present", () => {
      const rb = new RoaringBitmap32([1, 2, 3, 4, 5, 0xffffffff]);
      expect(rb.indexOf(6)).eq(-1);
      expect(rb.indexOf(-1)).eq(-1);
      expect(rb.indexOf(Number.MAX_SAFE_INTEGER)).eq(-1);
      expect(new RoaringBitmap32().indexOf(0)).eq(-1);
      expect(new RoaringBitmap32().indexOf(1)).eq(-1);
    });
    it("returns the index of the value", () => {
      const rb = new RoaringBitmap32([0, 3, 4, 6, 10, 11, 12, 0x7fffffff, 0xffffffff]);
      expect(rb.indexOf(0)).eq(0);
      expect(rb.indexOf(4)).eq(2);
      expect(rb.indexOf(11)).eq(5);
      expect(rb.indexOf(12)).eq(6);
      expect(rb.indexOf(0x7fffffff)).eq(7);
      expect(rb.indexOf(0xffffffff)).eq(8);
    });
    it("handles fromIndex correctly as normal arrays", () => {
      const array = [0, 3, 4, 6, 10, 11, 12, 0x7fffffff, 0xffffffff];
      const rb = new RoaringBitmap32(array);
      for (const j of [undefined, 0, 0.1, 0, 0.5, 1, 2, 1000, 0xffffffff, -0xffffffff, -1000, -2, -1]) {
        for (let i = 0; i < array.length; ++i) {
          const k = j !== undefined ? i + j : undefined;
          expect(rb.indexOf(array[i], k), `i:${i},j:${j}`).eq(array.indexOf(array[i], k));
        }
      }
    });
  });

  describe("lastIndexOf", () => {
    it("returns -1 if the value is not present", () => {
      const rb = new RoaringBitmap32([1, 2, 3, 4, 5, 0xffffffff]);
      expect(rb.lastIndexOf(6)).eq(-1);
      expect(rb.lastIndexOf(-1)).eq(-1);
      expect(rb.lastIndexOf(Number.MAX_SAFE_INTEGER)).eq(-1);
      expect(new RoaringBitmap32().lastIndexOf(0)).eq(-1);
      expect(new RoaringBitmap32().lastIndexOf(1)).eq(-1);
    });
    it("returns the index of the value", () => {
      const rb = new RoaringBitmap32([0, 3, 4, 6, 10, 11, 12, 0x7fffffff, 0xffffffff]);
      expect(rb.lastIndexOf(0)).eq(0);
      expect(rb.lastIndexOf(4)).eq(2);
      expect(rb.lastIndexOf(11)).eq(5);
      expect(rb.lastIndexOf(12)).eq(6);
      expect(rb.lastIndexOf(0x7fffffff)).eq(7);
      expect(rb.lastIndexOf(0xffffffff)).eq(8);
    });
    it("handles fromIndex correctly as normal arrays", () => {
      const array = [0, 3, 4, 6, 10, 11, 12, 0x7fffffff, 0xffffffff];
      const rb = new RoaringBitmap32(array);
      for (const j of [undefined, 0, 0.1, 0, 0.5, 1, 2, 1000, 0xffffffff, -0xffffffff, -1000, -2, -1]) {
        for (let i = 0; i < array.length; ++i) {
          const k = j !== undefined ? i + j : undefined;
          expect(rb.lastIndexOf(array[i], k), `i:${i},j:${j}`).eq(array.lastIndexOf(array[i], k));
        }
      }
    });
  });

  describe("functions are protected against illegal invocations", () => {
    it("allows subclassing", () => {
      class Subclass extends RoaringBitmap32 {
        public func(): void {
          this.add(1);
          this.addMany([3, 4]);
        }

        public add(value: number): this {
          super.add(value);
          super.add(value + 1);
          return this;
        }
      }

      const instance = new Subclass();
      expect(instance).to.be.instanceOf(RoaringBitmap32);
      instance.func();
      expect(instance.toArray()).deep.equal([1, 2, 3, 4]);
      instance.add(4);
      expect(instance.toArray()).deep.equal([1, 2, 3, 4, 5]);
    });

    it("throws with prototype.add direct call", () => {
      expect(() => {
        RoaringBitmap32.prototype.add(123);
      }).to.throw(Error);
    });

    it("throws with prototype.add.call on null", () => {
      expect(() => {
        RoaringBitmap32.prototype.add.call(null, 123);
      }).to.throw(Error);
    });

    it("throws with prototype.add.call on a wrong object", () => {
      expect(() => {
        RoaringBitmap32.prototype.add.call({}, 123);
      }).to.throw(Error);
    });

    it("throws when calling on the wrong native object", () => {
      const bitmap = new RoaringBitmap32();
      expect(() => {
        bitmap.add.call((bitmap as any)[Symbol.iterator as any](), 123);
      }).to.throw(Error);
    });

    it("allows prototype.add.call on the right object", () => {
      const bitmap = new RoaringBitmap32();
      RoaringBitmap32.prototype.add.call(bitmap, 123);
      expect(bitmap.toArray()).deep.equal([123]);
    });
  });

  it("implements Set<> interface properly", () => {
    const x = new RoaringBitmap32([1, 3]);
    x.add(2);
    expect(x.has(2)).eq(true);
    expect(Array.from(x.entries())).to.deep.equal([
      [1, 1],
      [2, 2],
      [3, 3],
    ]);
  });

  describe("forEach", () => {
    it("does nothing for an empty roaring bitmap", () => {
      let invoked = false;
      const bitmap = new RoaringBitmap32();
      bitmap.forEach(() => {
        invoked = true;
      });
      expect(invoked).to.equal(false);
    });

    it("invokes the function with a single item", () => {
      const invoked: any[] = [];
      const bitmap = new RoaringBitmap32([1]);
      bitmap.forEach((...args) => {
        invoked.push(args);
      });
      expect(invoked).to.deep.equal([[1, 0, bitmap]]);
      expect(typeof invoked[0][0]).eq("number");
    });

    it("invokes the function with multiple items", () => {
      const invoked: any[] = [];
      const bitmap = new RoaringBitmap32([2, 5, 1, 7, 6]);

      bitmap.forEach((...args) => {
        invoked.push(args);
      });
      expect(invoked).to.deep.equal([
        [1, 0, bitmap],
        [2, 1, bitmap],
        [5, 2, bitmap],
        [6, 3, bitmap],
        [7, 4, bitmap],
      ]);
    });

    it("handles exceptions well", () => {
      const invoked: any[] = [];
      const bitmap = new RoaringBitmap32([2, 5, 1, 7, 6]);
      expect(() =>
        bitmap.forEach((...args) => {
          invoked.push(args);
          if (invoked.length === 3) {
            throw new Error("expected");
          }
        }),
      ).to.throw("expected");
      expect(invoked).to.deep.equal([
        [1, 0, bitmap],
        [2, 1, bitmap],
        [5, 2, bitmap],
      ]);
    });
  });

  describe("pop", () => {
    it("returns the last element", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3]);
      expect(bitmap.pop()).eq(3);
      expect(bitmap.size).eq(2);
      expect(bitmap.pop()).eq(2);
      expect(bitmap.size).eq(1);
      expect(bitmap.pop()).eq(1);
      expect(bitmap.size).eq(0);
      expect(bitmap.pop()).eq(undefined);
    });
  });

  describe("shift", () => {
    it("returns the first element", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3]);
      expect(bitmap.shift()).eq(1);
      expect(bitmap.size).eq(2);
      expect(bitmap.shift()).eq(2);
      expect(bitmap.size).eq(1);
      expect(bitmap.shift()).eq(3);
      expect(bitmap.size).eq(0);
      expect(bitmap.shift()).eq(undefined);
    });
  });

  describe("map", () => {
    it("maps a bitmap", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3]);
      let counter = 0;
      const mapped = bitmap.map((x, index, set) => {
        expect(index).eq(counter++);
        expect(set).eq(bitmap);
        return x + 1;
      });
      expect(mapped).deep.equal([2, 3, 4]);
    });

    it("maps a bitmap with a context", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3]);
      const mapped = bitmap.map(function (this: number, x) {
        return x + this;
      }, 10);
      expect(mapped).deep.equal([11, 12, 13]);
    });
  });

  describe("filter", () => {
    it("filters a bitmap", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5, 6]);
      let counter = 0;
      const filtered = bitmap.filter((x, index, set) => {
        expect(index).eq(counter++);
        expect(set).eq(bitmap);
        return x % 2 === 0;
      });
      expect(filtered).deep.equal([2, 4, 6]);
    });
    it("filters a bitmap with a context", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5, 6]);
      const filtered = bitmap.filter(function (this: number, x) {
        return x % 2 === +this;
      }, 1);
      expect(filtered).deep.equal([1, 3, 5]);
    });
  });

  describe("reduce", () => {
    it("reduces a bitmap", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5, 6]);
      let counter = 0;
      const reduced = bitmap.reduce((acc, x, index, set) => {
        expect(index).eq(counter++);
        expect(set).eq(bitmap);
        return acc + x;
      }, 0);
      expect(reduced).eq(21);
    });
    it("reduces a bitmap with a undefined initial value", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5, 6]);
      let counter = 0;
      const reduced = bitmap.reduce((acc, x, index, set) => {
        expect(index).eq(counter++);
        expect(set).eq(bitmap);
        return acc + x;
      });
      expect(reduced).eq(21);
    });
  });

  describe("reduceRight", () => {
    it("reduces a bitmap", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5, 6]);
      let counter = bitmap.size;
      const reduced = bitmap.reduceRight((acc, x, index, set) => {
        expect(index).eq(--counter);
        expect(set).eq(bitmap);
        return acc + x;
      }, 0);
      expect(counter).eq(0);
      expect(reduced).eq(21);
    });
    it("reduces a bitmap with a undefined initial value", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5, 6]);
      let counter = bitmap.size;
      const reduced = bitmap.reduceRight((acc, x, index, set) => {
        expect(index).eq(--counter);
        expect(set).eq(bitmap);
        return acc + x;
      });
      expect(counter).eq(0);
      expect(reduced).eq(21);
    });
  });

  describe("findIndex", () => {
    it("finds the index of an item", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5, 6]);
      expect(bitmap.indexOf(4)).eq(3);
    });
    it("returns -1 if the item is not found", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5, 6]);
      expect(bitmap.indexOf(7)).eq(-1);
    });
  });

  describe("find", () => {
    it("finds an item", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5, 6]);
      expect(bitmap.find((x) => x === 4)).eq(4);
    });
    it("returns undefined if the item is not found", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5, 6]);
      expect(bitmap.find((x) => x === 7)).eq(undefined);
    });
  });

  describe("some", () => {
    it("returns true if any item matches", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5, 6]);
      expect(bitmap.some((x) => x === 4)).eq(true);
    });
    it("returns false if no item matches", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5, 6]);
      expect(bitmap.some((x) => x === 7)).eq(false);
    });
  });

  describe("every", () => {
    it("returns true if all items match", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5, 6]);
      expect(bitmap.every((x) => x < 7)).eq(true);
    });
    it("returns false if any item does not match", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5, 6]);
      expect(bitmap.every((x) => x < 6)).eq(false);
    });
  });

  describe("includes", () => {
    it("returns true if the item is included", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5, 6]);
      expect(bitmap.includes(4)).eq(true);
    });
    it("returns false if the item is not included", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5, 6]);
      expect(bitmap.includes(7)).eq(false);
    });
  });

  describe("toSorted", () => {
    it("returns a sorted array", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5, 6]);
      expect(bitmap.toSorted()).deep.equal([1, 2, 3, 4, 5, 6]);
    });
    it("applies the custom sorting", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5, 6]);
      expect(bitmap.toSorted((a, b) => (b & 1 ? a - b : b - a))).deep.equal([1, 3, 5, 6, 4, 2]);
    });
  });

  describe("toReversed", () => {
    it("returns a reversed array", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5, 6]);
      expect(bitmap.toReversed()).deep.equal([6, 5, 4, 3, 2, 1]);
    });

    it("accepts limit and skip arguments", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5, 6]);
      expect(bitmap.toReversed(3)).deep.equal([6, 5, 4]);
      expect(bitmap.toReversed(3, 1)).deep.equal([5, 4, 3]);
      expect(bitmap.toReversed(10, 10)).deep.equal([]);
    });

    it("can fill an existing array with offset and skip", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3, 4, 5, 6]);
      const target: number[] = [100];
      bitmap.toReversed(target, 2, 1);
      expect(target).deep.equal([100, 6, 5]);
      bitmap.toReversed(target, 2, 3, 2);
      expect(target).deep.equal([100, 6, 5, 4, 3]);
    });
  });
});
