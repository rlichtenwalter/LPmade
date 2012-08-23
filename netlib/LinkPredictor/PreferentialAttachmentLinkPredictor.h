#ifndef PREFERENTIALATTACHMENTLINKPREDICTOR_H
#define PREFERENTIALATTACHMENTLINKPREDICTOR_H

#include "../WeightedNetwork.h"
#include "LinkPredictor.h"

class PreferentialAttachmentLinkPredictor : public LinkPredictor {
	private:
	protected:
	public:
		PreferentialAttachmentLinkPredictor( const WeightedNetwork& );
		~PreferentialAttachmentLinkPredictor();
		double generateScore( vertex_t, vertex_t );
};

#endif
