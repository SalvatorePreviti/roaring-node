import { describe, it, expect } from "vitest";
import { asBuffer } from "../..";

describe("asBuffer", () => {
  it("wraps an ArrayBuffer", () => {
    const input = new ArrayBuffer(3);
    const output = asBuffer(input);
    expect(output).toBeInstanceOf(Buffer);
    expect(output.buffer).eq(input);
  });

  it("wraps an arraybuffer view", () => {
    for (const ctor of [
      Uint8Array,
      Uint16Array,
      Uint32Array,
      Int8Array,
      Int16Array,
      Int32Array,
      Float32Array,
      Float64Array,
      Uint8ClampedArray,
    ]) {
      const input = new ctor(3);
      const output = asBuffer(input);
      expect(output).toBeInstanceOf(Buffer);
      expect(output.buffer).eq(input.buffer);
      expect(output.byteOffset).eq(0);
      expect(output.byteLength).eq(input.byteLength);
    }
  });

  it("wraps an arraybuffer view with offset", () => {
    for (const ctor of [
      Uint8Array,
      Uint16Array,
      Uint32Array,
      Int8Array,
      Int16Array,
      Int32Array,
      Float32Array,
      Float64Array,
      Uint8ClampedArray,
    ]) {
      const sourceArrayBuffer = new ArrayBuffer(20 * ctor.BYTES_PER_ELEMENT);
      const input = new ctor(sourceArrayBuffer, 8, 8);
      const output = asBuffer(input);
      expect(output).toBeInstanceOf(Buffer);
      expect(output.buffer).eq(input.buffer);
      expect(output.byteOffset).eq(input.byteOffset);
      expect(output.byteLength).eq(input.byteLength);
    }
  });
});
