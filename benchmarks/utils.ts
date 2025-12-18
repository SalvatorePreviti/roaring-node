export function consume<T>(value: T): void {
  consume.value = value;
}

consume.value = 0 as unknown;
