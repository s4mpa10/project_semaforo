const tagInput = document.getElementById('tagInput');
const btnAcessar = document.getElementById('btnAcessar');
let readingInterval;

// Estado inicial do campo
const placeholderText = 'Aguardando Verificação';

function readTag() {
    // Atualiza o placeholder para indicar que a leitura está em andamento
    if (tagInput.value === '' || tagInput.value === placeholderText) {
        tagInput.value = 'Lendo tag...';
    }

    fetch('/read')
    .then(response => {
        if (!response.ok) {
        throw new Error('Erro na resposta do servidor.');
    }
        return response.json();
    })
    
    .then(data => {
    if (data && data.status === "success" && data.name) {
        tagInput.value = data.name;
        btnAcessar.disabled = false;
        clearInterval(readingInterval); //Realiza a parada da leitura

    } else {
        // Leitura não encontrou uma tag ou a resposta está 'vazia'
        tagInput.value = placeholderText;
        btnAcessar.disabled = true;
    }
    })


    .catch(error => {
        console.error('Houve um problema com a operação fetch:', error);
        tagInput.value = 'Erro na leitura';
        btnAcessar.disabled = true;
        clearInterval(readingInterval);
    });
} 

function startReading() {
    if (readingInterval) {
        clearInterval(readingInterval);
    };

    tagInput.value = placeholderText;
    btnAcessar.disabled = true;
    readingInterval = setInterval(readTag, 1000);
}

// Inicia a leitura automática a cada 2 segundos
window.onload = startReading;
