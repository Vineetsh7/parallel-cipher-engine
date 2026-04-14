import http.server
import socketserver
import json
import subprocess
import os

PORT = 8000
DIRECTORY = "ui"

class Handler(http.server.SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=DIRECTORY, **kwargs)

    def do_POST(self):
        if self.path == '/api/process':
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)
            data = json.loads(post_data)
            
            target_directory = data.get('directory', '').strip()
            action = data.get('action', '').strip() # "encrypt" or "decrypt"
            
            if not target_directory or action not in ['encrypt', 'decrypt']:
                self.send_response(400)
                self.send_header('Content-type', 'application/json')
                self.end_headers()
                response = {'status': 'error', 'message': 'Invalid input data.'}
                self.wfile.write(json.dumps(response).encode('utf-8'))
                return

            try:
                # Provide inputs sequentially via stdin for the c++ program
                process = subprocess.Popen(
                    ['pce.exe' if os.name == 'nt' else './pce'], 
                    stdin=subprocess.PIPE, 
                    stdout=subprocess.PIPE, 
                    stderr=subprocess.PIPE, 
                    text=True
                )
                
                stdout, stderr = process.communicate(input=f"{target_directory}\n{action}\n")
                
                self.send_response(200)
                self.send_header('Content-type', 'application/json')
                self.end_headers()
                
                response = {
                    'status': 'success',
                    'output': stdout,
                    'error': stderr
                }
                self.wfile.write(json.dumps(response).encode('utf-8'))
                
            except Exception as e:
                self.send_response(500)
                self.send_header('Content-type', 'application/json')
                self.end_headers()
                response = {'status': 'error', 'message': str(e)}
                self.wfile.write(json.dumps(response).encode('utf-8'))
        else:
            self.send_response(404)
            self.end_headers()

if __name__ == "__main__":
    print(f"Starting server... \nPlease open http://localhost:{PORT} in your web browser to access the Parallel Cipher Engine UI.")
    print("Press Ctrl+C to stop the server.")
    with socketserver.TCPServer(("", PORT), Handler) as httpd:
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\nServer shutting down.")
