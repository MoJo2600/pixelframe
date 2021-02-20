module.exports = (req, res, next) => {
  res.status(500).send({
    message: "Internal server error"
  });
};
