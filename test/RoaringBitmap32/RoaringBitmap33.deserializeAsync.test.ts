import RoaringBitmap32 from "../../RoaringBitmap32";
import { expect } from "chai";

describe("RoaringBitmap32 deserializeAsync", () => {
  describe("async/await", () => {
    describe("empty buffer", () => {
      it("deserializes an empty buffer (non portable)", async () => {
        const bitmap = await RoaringBitmap32.deserializeAsync(Buffer.from([]), false);
        expect(bitmap).to.be.instanceOf(RoaringBitmap32);
        expect(bitmap.size).eq(0);
      });

      it("deserializes an empty buffer (portable)", async () => {
        const bitmap = await RoaringBitmap32.deserializeAsync(Buffer.from([]), true);
        expect(bitmap).to.be.instanceOf(RoaringBitmap32);
        expect(bitmap.size).eq(0);
      });
    });

    describe("empty bitmap", () => {
      it("deserializes an empty bitmap (non portable)", async () => {
        const bitmap = await RoaringBitmap32.deserializeAsync(Buffer.from([1, 0, 0, 0, 0]), false);
        expect(bitmap).to.be.instanceOf(RoaringBitmap32);
        expect(bitmap.size).eq(0);
      });

      it("deserializes an empty bitmap (portable)", async () => {
        const bitmap = await RoaringBitmap32.deserializeAsync(Buffer.from([58, 48, 0, 0, 0, 0, 0, 0]), true);
        expect(bitmap).to.be.instanceOf(RoaringBitmap32);
        expect(bitmap.size).eq(0);
      });
    });

    it("deserializes simple bitmap", async () => {
      const values = [1, 2, 100, 101, 105, 109, 0x7fffffff, 0xfffffffe, 0xffffffff];

      const bufferNonPortable = new RoaringBitmap32(values).serialize(false);
      const bufferPortable = new RoaringBitmap32(values).serialize(true);

      const promises: Promise<RoaringBitmap32>[] = [];
      for (let i = 0; i < 10; ++i) {
        promises.push(RoaringBitmap32.deserializeAsync(bufferNonPortable, false));
        promises.push(RoaringBitmap32.deserializeAsync(bufferPortable, true));
      }

      for (const bitmap of await Promise.all(promises)) {
        expect(bitmap).to.be.instanceOf(RoaringBitmap32);
        expect(bitmap.toArray()).deep.equal(values);
      }
    });

    it("deserializes larger bitmap", async () => {
      const bitmap = new RoaringBitmap32();
      let rnd = 112043213;
      for (let i = 0; i < 1000; ++i) {
        rnd = ((rnd + i * 25253) * 3924461) >>> 0;
        bitmap.add(rnd);
      }
      const bufferNonPortable = bitmap.serialize(false);
      const bufferPortable = bitmap.serialize(true);

      const promises: Promise<RoaringBitmap32>[] = [];
      for (let i = 0; i < 5; ++i) {
        promises.push(RoaringBitmap32.deserializeAsync(bufferNonPortable, false));
        promises.push(RoaringBitmap32.deserializeAsync(bufferPortable, true));
      }

      const resolved = await Promise.all(promises);

      for (const b of resolved) {
        expect(b).to.be.instanceOf(RoaringBitmap32);
        expect(b.isEqual(bitmap)).eq(true);
      }
    });

    it("propagates deserialization errors", async () => {
      const wrongBuffer = Buffer.from([99, 98, 97]);
      const promise = RoaringBitmap32.deserializeAsync(wrongBuffer, false);
      let error: any;
      try {
        await promise;
      } catch (e) {
        error = e;
      }
      expect(error.message).eq("RoaringBitmap32 deserialization - invalid portable header byte");
    });
  });

  describe("callback", () => {
    describe("empty buffer", () => {
      it("deserializes an empty buffer (non portable)", () => {
        return new Promise<void>((done) => {
          expect(
            RoaringBitmap32.deserializeAsync(Buffer.from([]), false, (error, bitmap) => {
              expect(error).to.be.null;
              expect(bitmap).to.be.instanceOf(RoaringBitmap32);
              expect(bitmap!.size).eq(0);
              done();
            }),
          ).to.be.undefined;
        });
      });

      it("deserializes an empty buffer (portable)", () => {
        return new Promise<void>((done) => {
          expect(
            RoaringBitmap32.deserializeAsync(Buffer.from([]), true, (error, bitmap) => {
              expect(error).to.be.null;
              expect(bitmap).to.be.instanceOf(RoaringBitmap32);
              expect(bitmap!.size).eq(0);
              done();
            }),
          ).to.be.undefined;
        });
      });
    });

    describe("empty bitmap", () => {
      it("deserializes an empty bitmap (non portable)", () => {
        return new Promise<void>((done) => {
          expect(
            RoaringBitmap32.deserializeAsync(Buffer.from([1, 0, 0, 0, 0]), false, (error, bitmap) => {
              expect(error).to.be.null;
              expect(bitmap).to.be.instanceOf(RoaringBitmap32);
              expect(bitmap!.size).eq(0);
              done();
            }),
          ).to.be.undefined;
        });
      });

      it("deserializes an empty bitmap (portable)", () => {
        return new Promise<void>((done) => {
          expect(
            RoaringBitmap32.deserializeAsync(Buffer.from([58, 48, 0, 0, 0, 0, 0, 0]), true, (error, bitmap) => {
              expect(error).to.be.null;
              expect(bitmap).to.be.instanceOf(RoaringBitmap32);
              expect(bitmap!.size).eq(0);
              done();
            }),
          ).to.be.undefined;
        });
      });
    });

    it("deserializes simple bitmap", () => {
      return new Promise<void>((done) => {
        const values = [1, 2, 100, 101, 105, 109, 0x7fffffff, 0xfffffffe, 0xffffffff];

        const bufferNonPortable = new RoaringBitmap32(values).serialize(false);

        expect(
          RoaringBitmap32.deserializeAsync(bufferNonPortable, false, (error, bitmap) => {
            expect(error).to.be.null;
            expect(bitmap).to.be.instanceOf(RoaringBitmap32);
            expect(bitmap!.toArray()).deep.equal(values);
            done();
          }),
        ).to.be.undefined;
      });
    });

    it("propagates deserialization errors", () => {
      return new Promise<void>((done) => {
        const wrongBuffer = Buffer.from([99, 98, 97]);
        expect(
          RoaringBitmap32.deserializeAsync(wrongBuffer, false, (error, bitmap) => {
            expect(bitmap).to.be.undefined;
            expect(error!.message).eq("RoaringBitmap32 deserialization - invalid portable header byte");
            done();
          }),
        ).to.be.undefined;
      });
    });
  });
});
