#ifndef GRX_PIPELINE_H
#define GRX_PIPELINE_H
#include "translator.h"
static inline unsigned grx_features(const grx_config *c)
{return (c->texture_address?0x40u:0u)|(c->blend==GRX_OPAQUE?0u:2u);}
static inline unsigned grx_fragment(const grx_config *c)
{
    unsigned blend=c->blend==GRX_ALPHA?((6u<<19)|(7u<<23)):
                   c->blend==GRX_ADDITIVE?((1u<<19)|(1u<<23)):0;
    return 0x78000u|blend;
}
#endif
