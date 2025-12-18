import { describe, expect, it } from "vitest";
import RoaringBitmap32 from "../../RoaringBitmap32";

describe("RoaringBitmap32 fromAsync", () => {
  describe("awayt/async", () => {
    describe("empty", () => {
      it("creates an empty bitmap with undefined", async () => {
        const bitmap = await RoaringBitmap32.fromArrayAsync(undefined as any);
        expect(bitmap).to.be.instanceOf(RoaringBitmap32);
        expect(bitmap.isEmpty).eq(true);
      });

      it("creates an empty bitmap with null", async () => {
        const bitmap = await RoaringBitmap32.fromArrayAsync(null as any);
        expect(bitmap).to.be.instanceOf(RoaringBitmap32);
        expect(bitmap.isEmpty).eq(true);
      });

      it("creates an empty bitmap with an empty Uint32Array", async () => {
        const bitmap = await RoaringBitmap32.fromArrayAsync(new Uint32Array(0));
        expect(bitmap).to.be.instanceOf(RoaringBitmap32);
        expect(bitmap.isEmpty).eq(true);
      });

      it("creates an empty bitmap with an empty Int32Array", async () => {
        const bitmap = await RoaringBitmap32.fromArrayAsync(new Int32Array(0));
        expect(bitmap).to.be.instanceOf(RoaringBitmap32);
        expect(bitmap.isEmpty).eq(true);
      });

      it("creates an empty bitmap with an empty array", async () => {
        const bitmap = await RoaringBitmap32.fromArrayAsync([]);
        expect(bitmap).to.be.instanceOf(RoaringBitmap32);
        expect(bitmap.isEmpty).eq(true);
      });
    });

    describe("with data", () => {
      it("creates a bitmap with an Uint32Array", async () => {
        const values = [1, 2, 3, 4, 5];
        const bitmap = await RoaringBitmap32.fromArrayAsync(new Uint32Array(values));
        expect(bitmap).to.be.instanceOf(RoaringBitmap32);
        expect(bitmap.toArray()).deep.equal(values);
      });

      it("creates a bitmap with an Int32Array", async () => {
        const values = [1, 2, 3, 4, 5];
        const bitmap = await RoaringBitmap32.fromArrayAsync(new Int32Array(values));
        expect(bitmap).to.be.instanceOf(RoaringBitmap32);
        expect(bitmap.toArray()).deep.equal(values);
      });

      it("creates a bitmap with an array", async () => {
        const values = [1, 2, 3, 0x7fffffff, 0xffffffff];
        const bitmap = await RoaringBitmap32.fromArrayAsync(values);
        expect(bitmap).to.be.instanceOf(RoaringBitmap32);
        expect(bitmap.toArray()).deep.equal(values);
      });
    });
  });
});
