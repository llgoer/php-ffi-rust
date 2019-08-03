extern crate libc;
use libc::c_char;
use std::ffi::CString;
use std::iter;

#[no_mangle]
pub extern fn fib(n: i32) -> i32 {
  return match n {
    1 | 2 => 1,
    n => fib(n - 1) + fib(n - 2)
  }
}

#[no_mangle]
pub extern fn text_generate(length: u8) -> *mut c_char {
	let mut song = String::from("💣");
	song.extend(iter::repeat("na ").take(length as usize));
	song.push_str("Batman! 💣");

	let c_str_song = CString::new(song).unwrap();
	c_str_song.into_raw()
}

#[no_mangle]
pub extern fn text_free(s: *mut c_char) {
	unsafe {
		if s.is_null() {return}
		CString::from_raw(s)
	};
}