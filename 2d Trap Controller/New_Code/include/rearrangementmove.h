struct RearrangementMove {
	std::vector<bool> startingConfig;
	std::vector<bool> endingConfig;
	bool row; // 1 if row, 0 if col
	int dim; // the endex of the desired move
};

enum rearrange_method{
    HUNGARIAN,
    BALANCE_COMPRESS,
    DROP_IT_LIKE_ITS_HOT
};
