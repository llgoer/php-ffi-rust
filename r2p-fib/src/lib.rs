#[no_mangle]
pub extern fn fib(n: i32) -> i32 {
  return match n {
    1 | 2 => 1,
    n => fib(n - 1) + fib(n - 2)
  }
}