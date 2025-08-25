
document.addEventListener('DOMContentLoaded', (event) => {
document.getElementById("campoNome").style.display = "block";
document.getElementById("btnConfirmarNome").style.display = "block";
document.getElementById("mensagem-rfid").style.display = "none";
});

let cronometro;
let tempoRestante = 10;
let nomeParaGravar; // Variável para armazenar o nome digitado

// Nova função para realizar a requisição de escrita no servidor
function writeTag() {
    const mensagem = document.getElementById("mensagem-rfid");

    // Faz a requisição para a rota /write no servidor com o nome como parâmetro
    fetch(`/write?name=${encodeURIComponent(nomeParaGravar)}`)
    .then(response => response.text())
    .then(text => {
        clearInterval(cronometro); // Para o cronômetro
        
        if (text.includes("success")) {
        // Leitura e gravação bem-sucedidas
        mensagem.innerText = `Dados gravados com sucesso! O nome "${nomeParaGravar}" foi cadastrado.`;
        } else {
        // Falha na gravação ou timeout
        mensagem.innerText = text; // Exibe a mensagem de erro do servidor
        }

        // Reinicia a interface para um novo cadastro após um tempo
        setTimeout(() => {
        resetInterface();
        }, 3000);
    })
    .catch(error => {
        console.error('Erro na requisição:', error);
        clearInterval(cronometro);
        mensagem.innerText = "Erro na comunicação com o servidor.";
        setTimeout(() => {
        resetInterface();
        }, 3000);
    });
}

function resetInterface() {
    const mensagem = document.getElementById("mensagem-rfid");
    const campoNome = document.getElementById("campoNome");
    const btnConfirmarNome = document.getElementById("btnConfirmarNome");

    mensagem.style.display = "none";
    document.getElementById("nome").value = "";
    campoNome.style.display = "block";
    btnConfirmarNome.style.display = "block";
    tempoRestante = 10;
}

function confirmarNome() {
    const nome = document.getElementById("nome").value.trim();
    const mensagem = document.getElementById("mensagem-rfid");

    if (nome.length === 0) {
        alert("Por favor, digite um nome.");
        return;
    }

    nomeParaGravar = nome;

    document.getElementById("campoNome").style.display = "none";
    document.getElementById("btnConfirmarNome").style.display = "none";

    mensagem.innerText = `Olá, ${nomeParaGravar}! Agora, por favor, aproxime o cartão RFID. Você tem ${tempoRestante} segundos.`;
    mensagem.style.display = "block";

    // Inicia o cronômetro E a escrita da tag ao mesmo tempo
    cronometro = setInterval(() => {
    tempoRestante--;

    if (tempoRestante > 0) {
        mensagem.innerText = `Olá, ${nomeParaGravar}! Agora, por favor, aproxime o cartão RFID. Você tem ${tempoRestante} segundos.`;
    } else {
        clearInterval(cronometro);
        mensagem.innerText = "Tempo esgotado! Por favor, tente novamente.";
        setTimeout(() => {
        resetInterface();
        }, 3000);
    }
    }, 1000);

    // Dispara a função de escrita da tag
    writeTag();
}

// Funções adicionais para o resto da sua aplicação.
function listarFuncionarios() {
    alert("Aqui será exibida a lista de funcionários cadastrados.");
}