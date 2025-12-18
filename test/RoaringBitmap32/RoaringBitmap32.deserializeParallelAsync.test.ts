import { describe, expect, it } from "vitest";
import RoaringBitmap32 from "../../RoaringBitmap32";

describe("RoaringBitmap32 deserializeParallelAsync", () => {
  describe("async/await", () => {
    describe("one empty buffer", () => {
      it("deserializes an empty buffer (non portable)", async () => {
        const bitmap = await RoaringBitmap32.deserializeParallelAsync([Buffer.from([])], false);
        expect(bitmap).to.have.lengthOf(1);
        expect(bitmap[0]).to.be.instanceOf(RoaringBitmap32);
        expect(bitmap[0].size).eq(0);
      });

      it("deserializes an empty buffer (portable)", async () => {
        const bitmap = await RoaringBitmap32.deserializeParallelAsync([Buffer.from([])], true);
        expect(bitmap).to.have.lengthOf(1);
        expect(bitmap[0]).to.be.instanceOf(RoaringBitmap32);
        expect(bitmap[0].size).eq(0);
      });
    });

    describe("multiple empty buffers", () => {
      it("deserializes an empty buffer (non portable)", async () => {
        const bitmap = await RoaringBitmap32.deserializeParallelAsync(
          [Buffer.from([]), Buffer.from([]), Buffer.from([])],
          false,
        );
        expect(bitmap).to.have.lengthOf(3);
        for (let i = 0; i < 3; ++i) {
          expect(bitmap[i]).to.be.instanceOf(RoaringBitmap32);
          expect(bitmap[i].size).eq(0);
        }
      });

      it("deserializes an empty buffer (portable)", async () => {
        const bitmap = await RoaringBitmap32.deserializeParallelAsync(
          [Buffer.from([]), Buffer.from([]), Buffer.from([])],
          true,
        );
        expect(bitmap).to.have.lengthOf(3);
        for (let i = 0; i < 3; ++i) {
          expect(bitmap[i]).to.be.instanceOf(RoaringBitmap32);
          expect(bitmap[i].size).eq(0);
        }
      });
    });

    it("deserializes multiple buffers (non portable)", async () => {
      const sources = [];
      for (let i = 0; i < 10; ++i) {
        const array = [i, i + 10, i * 10000, i * 999999];
        for (let j = 0; j < i; ++j) {
          array.push(j);
        }
        sources.push(new RoaringBitmap32(array));
      }

      const result = await RoaringBitmap32.deserializeParallelAsync(
        sources.map((x) => x.serialize(false)),
        false,
      );
      expect(result).to.have.lengthOf(sources.length);
      for (let i = 0; i < sources.length; ++i) {
        expect(result[i].toArray()).deep.equal(sources[i].toArray());
      }
    });

    it("deserializes multiple buffers (portable)", async () => {
      const sources = [];
      for (let i = 0; i < 10; ++i) {
        const array = [i, i + 10, i * 10000, i * 999999];
        for (let j = 0; j < i; ++j) {
          array.push(j);
        }
        sources.push(new RoaringBitmap32(array));
      }

      const result = await RoaringBitmap32.deserializeParallelAsync(
        sources.map((x) => x.serialize(true)),
        true,
      );
      expect(result).to.have.lengthOf(sources.length);
      for (let i = 0; i < sources.length; ++i) {
        expect(result[i].toArray()).deep.equal(sources[i].toArray());
      }
    });
  });
});
