#ifndef CONTROL_INTERFACE_H
#define CONTROL_INTERFACE_H

void initializeAdwinConnection();

long sendTrigger(void);
long resetSendTrigger(void);
long resetRecvTrigger(void);
long waitForStartSequenceTrigger(void);
long waitForAnalyzeImageTrigger(void);


#endif