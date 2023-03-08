import RoaringBitmap32 from "../../RoaringBitmap32";
import { expect } from "chai";

const ERROR_FROZEN = "This bitmap is frozen and cannot be modified";

describe("RoaringBitmap32 frozen", () => {
  describe("freeze", () => {
    it("set isFrozen to true, return this, can be called multiple times", () => {
      const bitmap = new RoaringBitmap32();
      expect(bitmap.isFrozen).to.be.false;
      expect(bitmap.freeze()).to.equal(bitmap);
      expect(bitmap.isFrozen).to.be.true;

      expect(bitmap.freeze()).to.equal(bitmap);
      expect(bitmap.isFrozen).to.be.true;
    });

    it("throws when calling writable methods", () => {
      const bitmap = new RoaringBitmap32();
      bitmap.freeze();

      expect(() => bitmap.copyFrom([1])).to.throw(ERROR_FROZEN);
      expect(() => bitmap.add(1)).to.throw(ERROR_FROZEN);
      expect(() => bitmap.tryAdd(1)).to.throw(ERROR_FROZEN);
      expect(() => bitmap.addMany([1])).to.throw(ERROR_FROZEN);
      expect(() => bitmap.remove(1)).to.throw(ERROR_FROZEN);
      expect(() => bitmap.removeMany([1])).to.throw(ERROR_FROZEN);
      expect(() => bitmap.delete(1)).to.throw(ERROR_FROZEN);
      expect(() => bitmap.clear()).to.throw(ERROR_FROZEN);
      expect(() => bitmap.orInPlace([1])).to.throw(ERROR_FROZEN);
      expect(() => bitmap.andNotInPlace([1])).to.throw(ERROR_FROZEN);
      expect(() => bitmap.andInPlace([1])).to.throw(ERROR_FROZEN);
      expect(() => bitmap.xorInPlace([1])).to.throw(ERROR_FROZEN);
      expect(() => bitmap.flipRange(0, 10)).to.throw(ERROR_FROZEN);
      expect(() => bitmap.addRange(0, 9)).to.throw(ERROR_FROZEN);
      expect(() => bitmap.removeRange(0, 8)).to.throw(ERROR_FROZEN);

      const bitmap1 = new RoaringBitmap32();
      const bitmap2 = new RoaringBitmap32();
      bitmap1.freeze();
      expect(() => RoaringBitmap32.swap(bitmap1, bitmap2)).to.throw(ERROR_FROZEN);
      expect(() => RoaringBitmap32.swap(bitmap2, bitmap1)).to.throw(ERROR_FROZEN);
    });
  });

  describe("serializeAsync", () => {
    it("Is temporarily frozen while using serializeAsync", async () => {
      const bitmap = new RoaringBitmap32([1, 2, 3]);
      expect(bitmap.isFrozen).to.be.false;
      const promise = bitmap.serializeAsync("croaring");
      expect(bitmap.isFrozen).to.be.true;
      await promise;
      expect(bitmap.isFrozen).to.be.false;
      bitmap.add(4);
    });

    it('keep it frozen after calling "serializeAsync"', async () => {
      const bitmap = new RoaringBitmap32([1, 2, 3]);
      bitmap.freeze();
      expect(bitmap.isFrozen).to.be.true;
      const promise = bitmap.serializeAsync("croaring");
      expect(bitmap.isFrozen).to.be.true;
      await promise;
      expect(bitmap.isFrozen).to.be.true;
      expect(() => bitmap.add(4)).to.throw(ERROR_FROZEN);
    });
  });

  describe("unsafeFrozenView", () => {
    it("throws if the buffer is an invalid type", () => {
      expect(() => RoaringBitmap32.unsafeFrozenView({} as any, "unsafe_frozen_croaring")).to.throw();
      expect(() => RoaringBitmap32.unsafeFrozenView([] as any, "unsafe_frozen_croaring")).to.throw();
      expect(() => RoaringBitmap32.unsafeFrozenView(null as any, "unsafe_frozen_croaring")).to.throw();
      expect(() => RoaringBitmap32.unsafeFrozenView(undefined as any, "unsafe_frozen_croaring")).to.throw();
      expect(() => RoaringBitmap32.unsafeFrozenView(new Uint32Array(1), "unsafe_frozen_croaring")).to.throw();
    });

    it("throws if format is invalid", () => {
      expect(() => RoaringBitmap32.unsafeFrozenView(new Uint8Array(10), "a" as any)).to.throw();
      expect(() => RoaringBitmap32.unsafeFrozenView(new Uint8Array(10), "" as any)).to.throw();
      expect(() => RoaringBitmap32.unsafeFrozenView(new Uint8Array(10), undefined as any)).to.throw();
      expect(() => RoaringBitmap32.unsafeFrozenView(new Uint8Array(10), null as any)).to.throw();
    });

    it("can create a view from a serialized bitmap, and the view is frozen", () => {
      const values = [1, 2, 3, 100, 8772837, 0x7ffffff1, 0x7fffffff];
      const bitmap = new RoaringBitmap32(values);
      const serialized = bitmap.serialize("unsafe_frozen_croaring");
      expect(RoaringBitmap32.isBufferAligned(serialized)).to.be.true;
      const view = RoaringBitmap32.unsafeFrozenView(serialized, "unsafe_frozen_croaring");
      expect(view.isFrozen).to.be.true;
      expect(view.toArray()).to.deep.equal(values);
      expect(() => view.add(4)).to.throw(ERROR_FROZEN);
      expect(() => view.runOptimize()).to.throw(ERROR_FROZEN);
      expect(() => view.shrinkToFit()).to.throw(ERROR_FROZEN);
      expect(() => view.removeRunCompression()).to.throw(ERROR_FROZEN);

      const copy = view.clone();
      expect(copy.isFrozen).to.be.false;
      expect(copy.toArray()).to.deep.equal(values);
      expect(copy.tryAdd(4)).to.be.true;
    });

    const nodeVersion = parseInt(process.versions.node.split(".")[0], 10);
    if (nodeVersion >= 12) {
      it("can create a view from a serialized bitmap in a SharedArrayBuffer, and the view is frozen", () => {
        const values = [1, 2, 3, 100, 8772837, 0x7ffffff1, 0x7fffffff];
        const bitmap = new RoaringBitmap32(values);
        const sharedBuffer = RoaringBitmap32.bufferAlignedAllocShared(
          bitmap.getSerializationSizeInBytes("unsafe_frozen_croaring"),
        );
        expect(sharedBuffer.buffer).to.be.instanceOf(SharedArrayBuffer);
        const serialized = bitmap.serialize("unsafe_frozen_croaring", sharedBuffer);
        expect(serialized).to.eq(sharedBuffer);

        const view = RoaringBitmap32.unsafeFrozenView(sharedBuffer, "unsafe_frozen_croaring");
        expect(view.isFrozen).to.be.true;
        expect(view.toArray()).to.deep.equal(values);

        const copy = view.clone();
        expect(copy.isFrozen).to.be.false;
        expect(copy.toArray()).to.deep.equal(values);
        expect(copy.tryAdd(4)).to.be.true;
      });
    }
  });

  describe("asReadonlyView", () => {
    it("offers a readonly view", () => {
      const bitmap = new RoaringBitmap32([1, 2, 3]);
      const readonlyView = bitmap.asReadonlyView() as RoaringBitmap32;
      expect(readonlyView).to.not.eq(bitmap);
      expect(readonlyView.isFrozen).to.be.true;
      expect(readonlyView.asReadonlyView()).eq(readonlyView);
      expect(bitmap.asReadonlyView()).to.equal(readonlyView);

      expect(() => readonlyView.copyFrom([1])).to.throw(ERROR_FROZEN);
      expect(() => readonlyView.add(1)).to.throw(ERROR_FROZEN);
      expect(() => readonlyView.tryAdd(1)).to.throw(ERROR_FROZEN);
      expect(() => readonlyView.addMany([1])).to.throw(ERROR_FROZEN);
      expect(() => readonlyView.remove(1)).to.throw(ERROR_FROZEN);
      expect(() => readonlyView.removeMany([1])).to.throw(ERROR_FROZEN);
      expect(() => readonlyView.delete(1)).to.throw(ERROR_FROZEN);
      expect(() => readonlyView.clear()).to.throw(ERROR_FROZEN);
      expect(() => readonlyView.orInPlace([1])).to.throw(ERROR_FROZEN);
      expect(() => readonlyView.andNotInPlace([1])).to.throw(ERROR_FROZEN);
      expect(() => readonlyView.andInPlace([1])).to.throw(ERROR_FROZEN);
      expect(() => readonlyView.xorInPlace([1])).to.throw(ERROR_FROZEN);
      expect(() => readonlyView.flipRange(0, 10)).to.throw(ERROR_FROZEN);
      expect(() => readonlyView.addRange(0, 9)).to.throw(ERROR_FROZEN);
      expect(() => readonlyView.removeRange(0, 8)).to.throw(ERROR_FROZEN);
      expect(() => readonlyView.removeRunCompression()).to.throw(ERROR_FROZEN);
      expect(() => readonlyView.runOptimize()).to.throw(ERROR_FROZEN);
      expect(() => readonlyView.shrinkToFit()).to.throw(ERROR_FROZEN);

      bitmap.add(100);
      expect(readonlyView.toArray()).to.deep.equal([1, 2, 3, 100]);
      expect(readonlyView.size).to.equal(4);
      bitmap.add(200);
      expect(readonlyView.toArray()).to.deep.equal([1, 2, 3, 100, 200]);
      expect(readonlyView.size).to.equal(5);
      expect(readonlyView.size).to.equal(5);

      bitmap.freeze();
      expect(readonlyView.isFrozen).to.be.true;
      expect(bitmap.asReadonlyView()).eq(readonlyView);
    });
  });

  it("returns this if the bitmap is frozen", () => {
    const bitmap = new RoaringBitmap32();
    bitmap.freeze();
    expect(bitmap.asReadonlyView()).to.equal(bitmap);
  });
});
