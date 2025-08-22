function toggleDetalhes(botao) {
  const detalhes = botao.parentElement.nextElementSibling;
  const visivel = detalhes.style.display === 'block';
  detalhes.style.display = visivel ? 'none' : 'block';
  botao.textContent = visivel ? 'Detalhar' : 'Ver menos';
}

function toggleRele(num) {
  fetch('/toggle' + num)
    .then(response => {
      if (!response.ok) {
        throw new Error('Erro na requisição');
      }
      return response.text();
    })
    .then(state => {
      console.log('Relé ' + num + ' está ' + state);
    })
    .catch(error => console.error('Erro:', error));
}