#ifndef SIMPLEBRANCHPROFILER_H_
#define SIMPLEBRANCHPROFILER_H_

#include <string>
#include <vector>
#include <ocelot/ir/interface/PTXOperand.h>
#include "ocelot/ir/interface/Worker.h"

namespace ir {
	
	typedef struct {
		unsigned int    id;
		ir::ControlFlowGraph::iterator bb;
	} InstrumentedBBInfo;
	
	// This class instruments the kernel, to build the thread map.
	class SimpleBranchProfiler : public Worker {
	private:
		// We need this to create unique labels among all kernels in this module. 
		std::string kernelName;
	
		// We need a reference to cfg to inset new basic blocks.
		ir::ControlFlowGraph* kernelCFG;	
	
		// This is the first basic block in the kernel, that is, it is the
		// first basic block that will be traversed during the kernel execution.
		// This basic block is always empty. We need a reference to this
		// block in order to be able to insert instrumentation in the next
		// block, where we will initialize all the counters.
		ir::ControlFlowGraph::iterator entryBlock;
		
		// Block where instrumentation initialization will be inserted.
		ir::ControlFlowGraph::iterator initializationBlock;
		
		// We need a reference to the previously visited block, when visiting
		// the second basic block (where initialization code will be inserted)
		// and the exit block (we insert the printing code in the first block
		// before the exit block). 
		ir::ControlFlowGraph::iterator previousBlock;
		
		// We need a reference to the exitBlock in order to know that a basic
		// block, during the visitation, is indeed the exitBlock. We must keep
		// this reference because it is obtained from the CFG, and we do not
		// have access to the CFG during the traversal of basic blocks.
		ir::ControlFlowGraph::iterator exitBlock;

		// These are the virtual registers that will be used to produce
		// the results of the instrumentation. I am declaring them globally
		// because these variables need to be read in different calls of the
		// visit(BasicBlock*) method, for instance.
		ir::PTXOperand profArrayPt;
		ir::PTXOperand resultsArray;
		ir::PTXOperand dummy;

		// This array stores an id for each basic block. We use this id to
		// be able to match the result of the instrumentation with the
		// correct basic blocks.
		std::vector<InstrumentedBBInfo> instrumentedBlocksInfo;

		// The quantity of branches that we had to instrument. We instrument
		// every conditional branch, but only those, i.e: we do not instrument
		// branches that force all the threads to go to the same place, like
		// bra.uni.
		unsigned int instrumentedBranches;
		
		// The total number of basic blocks that we have visited. 
		unsigned int totalBlocks;
	public:
		SimpleBranchProfiler()
			: kernelCFG(NULL)
			, entryBlock(NULL)
			, initializationBlock(NULL)
			, previousBlock(NULL)
			, exitBlock(NULL)
			, profArrayPt("prof_data_pt", PTXOperand::Address, PTXOperand::u64)
			, resultsArray("%resultsArray", PTXOperand::Register, PTXOperand::u64)
			, dummy("%dummy", PTXOperand::Register, PTXOperand::u64)
			, instrumentedBranches(0)
			, totalBlocks(0)
		{}
		
		virtual ~SimpleBranchProfiler() {
		}
		
		virtual void process(ir::Module* m);

		virtual void postProcess(ir::Module* m) {}

		virtual void process(ir::Kernel* k);
		virtual void process(ir::ControlFlowGraph* cfg);

		virtual void postProcess(ir::ControlFlowGraph* cfg) {}

		virtual void process(ir::ControlFlowGraph::iterator bb);
		
		virtual void process(ir::Instruction* inst) {}
		
		/// This method adds code at the beginning of the kernel's control
		/// flow graph. The code is used to zero the memory positions where
		/// the results of the profiling will be inserted:
		///   if (tid == 0) {
		///     resultsArray[3*sizeof(long long)*0] = FIRST_BLOCK_ID
		///     resultsArray[3*sizeof(long long)*1] = SECOND_BLOCK_ID 
		///	    ...
		///   }
		void generateInitialization(ir::ControlFlowGraph::iterator bb);

		/// All that the method does is to store the id of a basic block in
		/// the cell array that corresponds to that basic block.
		void generateBasicBlockIdWrites(ir::ControlFlowGraph::iterator bb);

		/// This method adds to the CFG the instructions that will increment the
		/// counters of the basic block. There are two counters. The first
		/// keeps track of how many warps have gone through that basic block,
		/// and the second keeps track of how many divergencies had taken
		/// place at that branch. We only visit with this method basic blocks
		/// that end in a conditional branch.
		void instrumentBasicBlocks();

	};

}

#endif /*SIMPLEBRANCHPROFILER_H_*/
