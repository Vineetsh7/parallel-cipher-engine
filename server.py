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
                # Provide inputs via CLI arguments for the upgraded c++ program
                executable = 'pce.exe' if os.name == 'nt' else './pce'
                process = subprocess.Popen(
                    [executable, '--dir', target_directory, '--action', action], 
                    stdout=subprocess.PIPE, 
                    stderr=subprocess.STDOUT, 
                    text=True,
                    bufsize=1
                )
                
                self.send_response(200)
                self.send_header('Content-type', 'text/plain; charset=utf-8')
                self.end_headers()
                
                for line in iter(process.stdout.readline, ''):
                    if not line: break
                    try:
                        self.wfile.write(line.encode('utf-8'))
                        self.wfile.flush()
                    except BrokenPipeError:
                        break
                        
                process.stdout.close()
                process.wait()
                
            except Exception as e:
                try:
                    self.wfile.write(f"\n[Error]: {str(e)}".encode('utf-8'))
                    self.wfile.flush()
                except:
                    pass
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
