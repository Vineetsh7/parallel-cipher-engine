document.addEventListener('DOMContentLoaded', () => {
    const form = document.getElementById('engine-form');
    const submitBtn = document.getElementById('submit-btn');
    const btnText = submitBtn.querySelector('.btn-text');
    const loader = submitBtn.querySelector('.loader');
    
    const terminalContainer = document.getElementById('terminal-container');
    const terminalOutput = document.getElementById('terminal-output');

    form.addEventListener('submit', async (e) => {
        e.preventDefault();

        // Get values
        const directory = document.getElementById('directory').value;
        const action = document.querySelector('input[name="action"]:checked').value;

        // UI State: Loading
        btnText.classList.add('hidden');
        loader.classList.remove('hidden');
        submitBtn.style.pointerEvents = 'none';
        
        terminalContainer.classList.remove('hidden');
        terminalOutput.textContent = 'Initializing engine... \nExecuting ' + action + ' on: ' + directory + '\n';
        terminalOutput.classList.remove('error-text');

        try {
            const response = await fetch('/api/process', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ directory, action }),
            });

            if (!response.ok) {
                throw new Error("Server returned HTTP " + response.status);
            }

            const reader = response.body.getReader();
            const decoder = new TextDecoder("utf-8");
            
            // Stream the text line by line as it comes from the C++ binary
            while (true) {
                const { done, value } = await reader.read();
                if (done) break;
                
                terminalOutput.textContent += decoder.decode(value, { stream: true });
                
                // Keep auto-scrolling to bottom as text arrives
                const preElement = terminalOutput.parentElement;
                preElement.scrollTop = preElement.scrollHeight;
            }
        } catch (error) {
            terminalOutput.classList.add('error-text');
            terminalOutput.textContent += '\n\n[Network Error]: Backend server is not reachable. Ensure server.py is running.';
            console.error('Error:', error);
        } finally {
            // Restore UI State
            btnText.classList.remove('hidden');
            loader.classList.add('hidden');
            submitBtn.style.pointerEvents = 'auto';
            
            // Auto scroll to bottom of terminal
            const preElement = terminalOutput.parentElement;
            preElement.scrollTop = preElement.scrollHeight;
        }
    });
});
