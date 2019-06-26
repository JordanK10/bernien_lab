#ifndef REARRANGEMENT_MOVE_H
#define REARRANGEMENT_MOVE_H


struct RearrangementMove {
	std::vector<bool> startingConfig;
	std::vector<bool> endingConfig;
	bool row; // 1 if row, 0 if col
	int dim; // the endex of the desired move
};

#endif
