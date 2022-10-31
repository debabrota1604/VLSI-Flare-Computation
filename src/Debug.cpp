// Bit of code to understand and debug the code itself
#include "routing.h"

void 
PrintNode (TNODE* node, FILE* fptr)
{
  if (!node) {
    fprintf(fptr, "  Node ptr is Null!\n");
    return;
  }
  fprintf (fptr, "  Pin ID %hd - %s node, (%d,%d,%d), ", node->PinId,
           node->type==0? "source" : node->type == 1? "sink" : "internal",
           node->x, node->y, node->z);
  fprintf (fptr, "critical %hd, Cap %f, Delay %f\n", 
           node->critical, node->cap, node->delay);
}


void 
PrintSink (TNODE* sink, FILE* fptr)
{
  if (!sink) {
    fprintf(fptr, "  Sink ptr is Null!\n");
    return;
  }
  fprintf (fptr, "  Pin ID %hd - %s node, (%d,%d,%d), ", sink->PinId, 
           sink->type == 0? "source" : sink->type == 1? "sink" : "internal",
           sink->x, sink->y, sink->z);
  fprintf (fptr, "critical %hd, Cap %f, Delay %f\n", 
           sink->critical, sink->cap, sink->delay);
}

void 
PrintNet (TNET* net, FILE* fptr)
{
   int i;
   fprintf (fptr, "Net ID %hd, drive %f, total cap %f\n", net->Id, net->drive, net->C_0);
   for (i=1; i<= net->Number; i++)
     PrintNode (net->Sink + i, fptr);
}
