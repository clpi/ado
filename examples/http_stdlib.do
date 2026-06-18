# HTTP Stdlib Example
# Demonstrates HTTP functions in Ado (use ado_ prefix)

fn main() {
  print("=== HTTP Stdlib Tests ===")
  
  let code = ado_http_get("https://httpbin.org/get")
  print("GET status:", code)
  
  let post_code = ado_http_post("https://httpbin.org/post", "hello from ado")
  print("POST status:", post_code)
  
  let put_code = ado_http_put("https://httpbin.org/put", "updated")
  print("PUT status:", put_code)
  
  let del_code = ado_http_delete("https://httpbin.org/delete")
  print("DELETE status:", del_code)
  
  let status = ado_http_status("https://httpbin.org/status/200")
  print("Status check:", status)
  
  print("")
  print("All HTTP tests completed!")
  return 0
}
