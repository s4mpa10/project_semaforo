// A sua função toggleDetalhes está correta e pode ser mantida
function toggleDetalhes(botao) {
    const detalhes = botao.parentElement.nextElementSibling;
    const visivel = detalhes.style.display === 'block';
    detalhes.style.display = visivel ? 'none' : 'block';
    botao.textContent = visivel ? 'Detalhar' : 'Ver menos';
}

// ===================================
// LÓGICA NOVA PARA CONTROLE DO SEMÁFORO
// ===================================

document.addEventListener('DOMContentLoaded', () => {
 // Adiciona o evento de clique a todos os botões de ação (Iniciar, Pausar, Finalizar)
 const botoes = document.querySelectorAll('.iniciar, .pausar, .finalizar');

 botoes.forEach(botao => {
  botao.addEventListener('click', () => {
   let rota;
   if (botao.classList.contains('iniciar')) {
    rota = '/iniciar';
   } else if (botao.classList.contains('pausar')) {
    rota = '/pausar';
   } else if (botao.classList.contains('finalizar')) {
    rota = '/finalizar';
   }

   if (rota) {
    fetch(rota)
     .then(response => {
      if (!response.ok) {
       throw new Error('Erro na requisição');
      }
      return response.text();
     })
     .then(data => {
      console.log('Comando enviado:', data);
     })
     .catch(error => console.error('Erro na conexão:', error));
   }
  });
 });
});