function toggleDetalhes(botao) {
    const detalhes = botao.parentElement.nextElementSibling;
    const visivel = detalhes.style.display === 'block';
    detalhes.style.display = visivel ? 'none' : 'block';
    botao.textContent = visivel ? 'Detalhar' : 'Ver menos';
}