export default days => new Date(new Date().setDate(new Date().getDate() - days))
  .toISOString();
