#ifndef REARRANGEMENT_MOVE_H
#define REARRANGEMENT_MOVE_H


struct RearrangementMove {
	std::vector<bool> startingConfig;
	std::vector<bool> endingConfig;
	bool row; // 1 if row, 0 if col
	int dim; // the endex of the desired move
};

<<<<<<< HEAD:2d_Trap_Controller/New_Code/include/rearrangementmove.h
enum rearrange_method{
    HUNGARIAN,
    BALANCE_COMPRESS,
    DROP_IT_LIKE_ITS_HOT
};
=======
#endif
>>>>>>> origin/aod_Dev:2d Trap Controller/New_Code/include/rearrangementmove.h
