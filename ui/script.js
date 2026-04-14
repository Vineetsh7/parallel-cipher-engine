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

            const data = await response.json();

            if (data.status === 'success') {
                let outputStr = "";
                if (data.output) outputStr += data.output;
                if (data.error && data.error.trim() !== '') outputStr += "\n[STDERR]: " + data.error;
                
                terminalOutput.textContent += '\n' + outputStr;
                terminalOutput.textContent += '\n\n[Process Completed Successfully]';
            } else {
                terminalOutput.classList.add('error-text');
                terminalOutput.textContent += '\n\n[Error]: ' + (data.message || 'Unknown error occurred.');
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
