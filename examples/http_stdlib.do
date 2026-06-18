# HTTP Stdlib Example
# Demonstrates HTTP functions in Ado

fn main() {
  print("=== HTTP Stdlib Tests ===")
  
  let code = http_get("https://httpbin.org/get")
  print("GET status:", code)
  
  let post_code = http_post("https://httpbin.org/post", "hello from ado")
  print("POST status:", post_code)
  
  let put_code = http_put("https://httpbin.org/put", "updated")
  print("PUT status:", put_code)
  
  let del_code = http_delete("https://httpbin.org/delete")
  print("DELETE status:", del_code)
  
  let status = http_status("https://httpbin.org/status/200")
  print("Status check:", status)
  
  print("")
  print("All HTTP tests completed!")
  return 0
}
